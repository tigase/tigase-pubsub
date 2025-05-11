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
do $$
begin
if exists (select 1 from information_schema.columns where table_catalog = current_database() and table_schema = 'public' and table_name = 'tig_pubsub_nodes' and column_name = 'creator_id') then
    if not exists (select 1 from information_schema.columns where table_catalog = current_database() and table_schema = 'public' and table_name = 'tig_pubsub_nodes' and column_name = 'creator') then
        alter table tig_pubsub_nodes add column creator varchar(2049);
    end if;

    if exists(select 1 from tig_pubsub_nodes where creator is null and creator_id is not null) then
        update tig_pubsub_nodes as n
            set creator = j.jid
            from tig_pubsub_jids j
            where n.creator is null and j.jid_id = n.creator_id;
    end if;

    alter table tig_pubsub_nodes drop column creator_id;
end if;
end$$;
-- QUERY END:

-- QUERY START:
do $$
begin
if exists (select 1 from information_schema.columns where table_catalog = current_database() and table_schema = 'public' and table_name = 'tig_pubsub_items' and column_name = 'publisher_id') then
    if not exists (select 1 from information_schema.columns where table_catalog = current_database() and table_schema = 'public' and table_name = 'tig_pubsub_items' and column_name = 'publisher') then
        alter table tig_pubsub_items add column publisher varchar(2049);
    end if;

    if exists(select 1 from tig_pubsub_items where publisher is null and publisher_id is not null) then
        update tig_pubsub_items i
            set publisher = j.jid
            from tig_pubsub_jids j
            where i.publisher is null and j.jid_id = i.publisher_id;
    end if;

    alter table tig_pubsub_items drop column publisher_id;
end if;
end$$;
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubCreateNode(_service_jid varchar(2049), _node_name varchar(1024), _node_type int, _node_creator varchar(2049), _node_conf text, _collection_id bigint, _ts timestamp with time zone, _domain varchar(1024), _createService int) returns bigint as $$
declare
    _service_id bigint;
    _node_creator_id bigint;
    _node_id bigint;
begin
    select TigPubSubEnsureServiceJid(_service_jid, _domain, _createService) into _service_id;
    insert into tig_pubsub_nodes (service_id, name, "type", creator, creation_date, configuration, collection_id)
        values (_service_id, _node_name, _node_type, _node_creator, _ts, _node_conf, _collection_id);
    select currval('tig_pubsub_nodes_node_id_seq') into _node_id;
    return _node_id;
end;
$$ LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubGetNodeMeta(_service_jid varchar(2049), _node_name varchar(1024)) returns table (
    node_id bigint,
    configuration text,
    creator varchar(2049),
    creation_date timestamp with time zone
) as $$
begin
    return query select n.node_id, n.configuration, n.creator, n.creation_date
        from tig_pubsub_nodes n
            inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
            where lower(sj.service_jid) = lower(_service_jid) and n.name = _node_name;
end ;
$$ LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubWriteItem(bigint,varchar(1024),varchar(2049),text, timestamp with time zone, varchar(36)) returns void as $$
declare
    _node_id alias for $1;
	_item_id alias for $2;
	_publisher alias for $3;
	_item_data alias for $4;
	_ts alias for $5;
	_uuid alias for $6;
begin
	if exists (select 1 from tig_pubsub_items where node_id = _node_id and id = _item_id) then
        update tig_pubsub_items set update_date = _ts, data = _item_data, uuid = uuid(_uuid)
            where node_id = _node_id and id = _item_id;
    else
        insert into tig_pubsub_items (node_id, id, creation_date, update_date, publisher, data, uuid)
            values (_node_id, _item_id, _ts, _ts, _publisher, _item_data, uuid(_uuid));
    end if;
end;
$$ LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubRemoveService(varchar(2049)) returns void as $$
delete from tig_pubsub_items where node_id in (
    select n.node_id from tig_pubsub_nodes n
                              inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
    where lower(sj.service_jid) = lower($1));
delete from tig_pubsub_affiliations where node_id in (
    select n.node_id from tig_pubsub_nodes n
                              inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
    where lower(sj.service_jid) = lower($1));
delete from tig_pubsub_subscriptions where node_id in (
    select n.node_id from tig_pubsub_nodes n
                              inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
    where lower(sj.service_jid) = lower($1));
delete from tig_pubsub_nodes where node_id in (
    select n.node_id from tig_pubsub_nodes n
                              inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
    where lower(sj.service_jid) = lower($1));
delete from tig_pubsub_service_jids where lower(service_jid) = lower($1);
delete from tig_pubsub_affiliations where jid_id in (select j.jid_id from tig_pubsub_jids j where lower(j.jid) = lower($1));
delete from tig_pubsub_subscriptions where jid_id in (select j.jid_id from tig_pubsub_jids j where lower(j.jid) = lower($1));
delete from tig_pubsub_jids where lower(jid) = lower($1);
$$ LANGUAGE SQL;
-- QUERY END: