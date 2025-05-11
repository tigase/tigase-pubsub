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

delimiter ;

-- QUERY START:
drop procedure if exists TigPubSubUpgrade;
-- QUERY END:

delimiter //

-- QUERY START:
create procedure TigPubSubUpgrade()
begin
    if exists (select 1 from information_schema.columns where table_schema = database() and table_name = 'tig_pubsub_nodes' and column_name = 'creator_id') then
        if not exists (select 1 from information_schema.columns where table_schema = database() and table_name = 'tig_pubsub_nodes' and column_name = 'creator') then
            alter table tig_pubsub_nodes add column creator varchar(2049);
        end if;
        if exists(select 1 from tig_pubsub_nodes where creator is null and creator_id is not null) then
            update tig_pubsub_nodes n
            join tig_pubsub_jids j ON j.jid_id = n.creator_id
            set n.creator = j.jid
            where n.creator is null;
        end if;
        select constraint_name into @constraint_name from information_schema.key_column_usage where referenced_table_name = 'tig_pubsub_jids' and table_name = 'tig_pubsub_nodes' and constraint_schema = database() and column_name = 'creator_id';
        if (@constraint_name is not null) then
            set @s = concat("alter table tig_pubsub_nodes drop constraint ", @constraint_name);
            prepare stmt from @s;
            execute stmt;
            deallocate prepare stmt;
        end if;
        alter table tig_pubsub_nodes drop column creator_id;
    end if;

    if exists (select 1 from information_schema.columns where table_schema = database() and table_name = 'tig_pubsub_items' and column_name = 'publisher_id') then
        if not exists (select 1 from information_schema.columns where table_schema = database() and table_name = 'tig_pubsub_items' and column_name = 'publisher') then
            alter table tig_pubsub_items
                add column publisher varchar(2049);
        end if;
        if exists(select 1 from tig_pubsub_items where publisher is null and publisher_id is not null) then
            update tig_pubsub_items i
            join tig_pubsub_jids j ON j.jid_id = i.publisher_id
            set i.publisher = j.jid
            where i.publisher is null;
        end if;
        select constraint_name into @constraint_name from information_schema.key_column_usage where referenced_table_name = 'tig_pubsub_jids' and table_name = 'tig_pubsub_items' and constraint_schema = database() and column_name = 'publisher_id';
        if (@constraint_name is not null) then
            set @s = concat("alter table tig_pubsub_items drop constraint ", @constraint_name);
            prepare stmt from @s;
            execute stmt;
            deallocate prepare stmt;
        end if;
        alter table tig_pubsub_items drop column publisher_id;
    end if;
end //
-- QUERY END:

delimiter ;

-- QUERY START:
call TigPubSubUpgrade();
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubUpgrade;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubCreateNode;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubGetNodeMeta;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubWriteItem;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubWriteItem;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigPubSubRemoveService
-- QUERY END:
    
delimiter //

-- QUERY START:
create procedure TigPubSubCreateNode(_service_jid varchar(2049), _node_name varchar(1024) charset utf8mb4 collate utf8mb4_bin, _node_type int,
                                     _node_creator varchar(2049), _node_conf mediumtext charset utf8mb4 collate utf8mb4_bin, _collection_id bigint, _ts timestamp(6), _domain varchar(1024), _createService int)
begin
	declare _service_id bigint;
	declare _node_id bigint;

    -- DO NOT REMOVE, required for properly handle exceptions within transactions!
    DECLARE exit handler for sqlexception
    BEGIN
        -- ERROR
        ROLLBACK;
    RESIGNAL;
    END;

    START TRANSACTION;
    call TigPubSubEnsureServiceJid(_service_jid, _domain, _createService, _service_id);

    insert into tig_pubsub_nodes (service_id,name,name_sha1,`type`,creator, creation_date, configuration,collection_id)
        values (_service_id, _node_name, sha1(_node_name), _node_type, _node_creator, _ts, _node_conf, _collection_id);
    select LAST_INSERT_ID() into _node_id;
    select _node_id as node_id;
    COMMIT;
end //
-- QUERY END:

-- QUERY START:
create procedure TigPubSubGetNodeMeta(_service_jid varchar(2049), _node_name varchar(1024) charset utf8mb4 collate utf8mb4_bin)
begin
    select n.node_id, n.configuration, n.creator, n.creation_date
    from tig_pubsub_nodes n
        inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
        where sj.service_jid_sha1 = SHA1(LOWER(_service_jid)) and n.name_sha1 = SHA1(_node_name)
            and n.name = _node_name;
end //
-- QUERY END:

-- QUERY START:
create procedure TigPubSubWriteItem(_node_id bigint, _item_id varchar(1024) charset utf8mb4 collate utf8mb4_bin, _publisher varchar(2049),
                                    _item_data mediumtext charset utf8mb4, _ts timestamp(6), _uuid varchar(36))
begin
	-- DO NOT REMOVE, required for properly handle exceptions within transactions!
    DECLARE exit handler for sqlexception
    BEGIN
        -- ERROR
        ROLLBACK;
    RESIGNAL;
    END;

    START TRANSACTION;
    insert into tig_pubsub_items (node_id, id_sha1, id, creation_date, update_date, publisher, data, uuid)
        values (_node_id, SHA1(_item_id), _item_id, _ts, _ts, _publisher, _item_data, TigPubSubUuidToOrdered(_uuid))
        on duplicate key update publisher = _publisher, data = _item_data, update_date = _ts, uuid = TigPubSubUuidToOrdered(_uuid);
COMMIT;
end //
-- QUERY END:

-- QUERY START:
create procedure TigPubSubRemoveService(_service_jid varchar(2049))
begin
    -- DO NOT REMOVE, required for properly handle exceptions within transactions!
    DECLARE exit handler for sqlexception
    BEGIN
        -- ERROR
        ROLLBACK;
    RESIGNAL;
    END;

    START TRANSACTION;
    select * from tig_pubsub_service_jids where service_jid_sha1 = SHA1(LOWER(_service_jid)) for update;
    select * from tig_pubsub_jids where jid_sha1 = SHA1(LOWER(_service_jid)) for update;
    delete i
	    from tig_pubsub_items i
	    join tig_pubsub_nodes n on n.node_id = i.node_id
	    join tig_pubsub_service_jids s on n.service_id = s.service_id
	    where s.service_jid_sha1 = SHA1(LOWER(_service_jid));
	delete a
	    from tig_pubsub_affiliations a
	    join tig_pubsub_nodes n on n.node_id = a.node_id
	    join tig_pubsub_service_jids s on n.service_id = s.service_id
	    where s.service_jid_sha1 = SHA1(LOWER(_service_jid));
	delete sub
	    from tig_pubsub_subscriptions sub
	    join tig_pubsub_nodes n on n.node_id = sub.node_id
	    join tig_pubsub_service_jids s on n.service_id = s.service_id
	    where s.service_jid_sha1 = SHA1(LOWER(_service_jid));
	delete m
	    from tig_pubsub_mam m
	    join tig_pubsub_nodes n on n.node_id = m.node_id
	    join tig_pubsub_service_jids s on n.service_id = s.service_id
	    where s.service_jid_sha1 = SHA1(LOWER(_service_jid));
	delete n
	    from tig_pubsub_nodes n
	    join tig_pubsub_service_jids s on n.service_id = s.service_id
	    where s.service_jid_sha1 = SHA1(LOWER(_service_jid));
    delete from tig_pubsub_service_jids where service_jid_sha1 = SHA1(LOWER(_service_jid));
    delete a
	    from tig_pubsub_affiliations a
	    join tig_pubsub_jids j on j.jid_id = a.jid_id
	    where j.jid_sha1 = SHA1(LOWER(_service_jid));
	delete s
	    from tig_pubsub_subscriptions s
	    join tig_pubsub_jids j on j.jid_id = s.jid_id
	    where j.jid_sha1 = SHA1(LOWER(_service_jid));
	delete from tig_pubsub_jids WHERE jid_sha1 = SHA1(LOWER(_service_jid));
    COMMIT;
end //
-- QUERY END:

delimiter ;