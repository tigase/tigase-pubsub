--
-- Tigase PubSub - Publish Subscribe component for Tigase
-- Copyright (C) 2008 Tigase, Inc. (office@tigase.com)
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Affero General Public License as published by
-- the Free Software Foundation, version 3 of the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Affero General Public License for more details.
--
-- You should have received a copy of the GNU Affero General Public License
-- along with this program. Look for COPYING file in the top folder.
-- If not, see http://www.gnu.org/licenses/.
--

-- QUERY START:
IF NOT EXISTS (SELECT 1 FROM sys.columns WHERE name = 'creator' AND object_id = object_id('dbo.tig_pubsub_nodes'))
BEGIN
    ALTER TABLE [tig_pubsub_nodes] ADD [creator] [nvarchar](2049);
END
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT 1 FROM sys.columns WHERE name = 'creator_id' AND object_id = object_id('dbo.tig_pubsub_nodes'))
BEGIN
    if exists(select 1 from tig_pubsub_nodes where creator is null and creator_id is not null)
    begin
        update n
        set n.creator = j.jid
        from tig_pubsub_nodes n
        join tig_pubsub_jids j ON j.jid_id = n.creator_id
        where n.creator is null;
    end

    ALTER TABLE [tig_pubsub_nodes] DROP CONSTRAINT [FK_tig_pubsub_nodes_creator_id];

    ALTER TABLE [tig_pubsub_nodes] DROP COLUMN [creator_id];
END
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS (SELECT 1 FROM sys.columns WHERE name = 'publisher' AND object_id = object_id('dbo.tig_pubsub_items'))
BEGIN
    ALTER TABLE [tig_pubsub_items] ADD [publisher] [nvarchar](2049);
END
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT 1 FROM sys.columns WHERE name = 'publisher_id' AND object_id = object_id('dbo.tig_pubsub_items'))
BEGIN
    if exists(select 1 from tig_pubsub_items where publisher is null and publisher_id is not null)
    begin
        update i
        set i.publisher = j.jid
        from tig_pubsub_items i
        join tig_pubsub_jids j ON j.jid_id = i.publisher_id
        where i.publisher is null;
    end

    ALTER TABLE [tig_pubsub_items] DROP CONSTRAINT [FK_tig_pubsub_items_publisher_id];

    ALTER TABLE [tig_pubsub_items] DROP COLUMN [publisher_id];
END
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubCreateNode')
    DROP PROCEDURE TigPubSubCreateNode
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubCreateNode
    @_service_jid nvarchar(2049),
	@_node_name nvarchar(1024),
	@_node_type int,
	@_node_creator nvarchar(2049),
	@_node_conf nvarchar(max),
	@_collection_id bigint,
	@_ts datetime,
	@_domain nvarchar(1024),
	@_autocreateService int
AS
begin
	declare @_service_id bigint;

	-- temporarily disable transaction as they don't work with MS SQL JDBC driver
    --     begin transaction;
    exec TigPubSubEnsureServiceJid @_service_jid=@_service_jid, @_domain=@_domain, @_autocreateService=@_autocreateService, @_service_id=@_service_id output;

    insert into dbo.tig_pubsub_nodes (service_id, name, name_sha1, type, creator, creation_date, configuration, collection_id)
        values (@_service_id, @_node_name, HASHBYTES('SHA1', @_node_name), @_node_type, @_node_creator, @_ts, @_node_conf, @_collection_id);

    select @@IDENTITY as node_id;

    --     commit transaction;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeMeta')
    DROP PROCEDURE TigPubSubGetNodeMeta
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeMeta
    @_service_jid nvarchar(2049),
	@_node_name nvarchar(1024)
AS
begin
    select n.node_id, n.configuration, n.creator, n.creation_date
    from tig_pubsub_nodes n
        inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
        where sj.service_jid_sha1 = HASHBYTES('SHA1', LOWER(@_service_jid)) and n.name_sha1 = HASHBYTES('SHA1', @_node_name)
            and n.name = @_node_name;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubWriteItem')
    DROP PROCEDURE TigPubSubWriteItem
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubWriteItem
    @_node_id bigint,
	@_item_id nvarchar(1024),
	@_publisher nvarchar(2049),
	@_item_data ntext,
	@_ts datetime,
	@_uuid nvarchar(36)
AS
begin
    SET NOCOUNT ON;

	-- Update the row if it exists.
    UPDATE tig_pubsub_items
        SET publisher = @_publisher, data = @_item_data, update_date = @_ts, uuid = CONVERT(uniqueidentifier, @_uuid)
        WHERE tig_pubsub_items.node_id = @_node_id
            and tig_pubsub_items.id_index = CAST(@_item_id as nvarchar(255))
            and tig_pubsub_items.id = @_item_id;
    -- Insert the row if the UPDATE statement failed.
    IF (@@ROWCOUNT = 0 )
    BEGIN
        BEGIN TRY
            insert into tig_pubsub_items (node_id, id, id_sha1, creation_date, update_date, publisher, data, uuid)
            select @_node_id, @_item_id, HASHBYTES('SHA1',@_item_id), @_ts, @_ts, @_publisher, @_item_data, CONVERT(uniqueidentifier, @_uuid) where not exists(
				select 1 from tig_pubsub_items where node_id = @_node_id AND id_sha1 = HASHBYTES('SHA1',@_item_id));
        END TRY
        BEGIN CATCH
            IF ERROR_NUMBER() <> 2627
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
                        select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
                        raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
        END CATCH
    END
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubRemoveService')
DROP PROCEDURE TigPubSubRemoveService
-- QUERY END:
    GO

-- QUERY START:
create procedure dbo.TigPubSubRemoveService
    @_service_jid nvarchar(2049)
AS
begin
	declare @_service_id bigint;

    select @_service_id=service_id from tig_pubsub_service_jids where service_jid_sha1 = HASHBYTES('SHA1', @_service_jid);

    delete from dbo.tig_pubsub_items where node_id in (
        select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
    delete from dbo.tig_pubsub_subscriptions where node_id in (
        select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);;
    delete from dbo.tig_pubsub_affiliations where node_id in (
        select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
    delete from dbo.tig_pubsub_nodes where node_id in (
        select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
    delete from tig_pubsub_service_jids where service_id = @_service_id;
    delete from tig_pubsub_affiliations where jid_id in (
        select j.jid_id from tig_pubsub_jids j where j.jid_sha1 = HASHBYTES('SHA1', @_service_jid) and j.jid = @_service_jid);
    delete from tig_pubsub_subscriptions where jid_id in (
        select j.jid_id from tig_pubsub_jids j where j.jid_sha1 = HASHBYTES('SHA1', @_service_jid) and j.jid = @_service_jid);
    delete from tig_pubsub_jids where jid_sha1 = HASHBYTES('SHA1', @_service_jid);
end
-- QUERY END:
GO
