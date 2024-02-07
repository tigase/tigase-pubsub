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
create or replace function TigPubSubMamUpdateItem(bigint, varchar(36), text) returns void as $$
declare
	_node_id alias for $1;
	_uuid alias for $2;
	_item_data alias for $3;
begin
	update tig_pubsub_mam
        set data = _item_data
    where
        node_id = _node_id
        and uuid = uuid(_uuid);
end;
$$ LANGUAGE 'plpgsql';
-- QUERY END: