/**
 * Tigase PubSub - Publish Subscribe component for Tigase
 * Copyright (C) 2008 Tigase, Inc. (office@tigase.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. Look for COPYING file in the top folder.
 * If not, see http://www.gnu.org/licenses/.
 */
package tigase.pubsub.repository.stateless;

import tigase.component.exceptions.RepositoryException;
import tigase.pubsub.repository.IItems;
import tigase.pubsub.repository.IPubSubDAO;
import tigase.xml.Element;
import tigase.xmpp.jid.BareJID;

import java.util.Date;
import java.util.List;

class Items
		implements IItems {

	private final IPubSubDAO dao;

	private final long nodeId;

	private final String nodeName;

	private final BareJID serviceJid;

	public Items(long nodeId, BareJID serviceJid, String nodeName, IPubSubDAO dao) {
		this.nodeId = nodeId;
		this.dao = dao;
		this.nodeName = nodeName;
		this.serviceJid = serviceJid;
	}

	@Override
	public void deleteItem(String id) throws RepositoryException {
		this.dao.deleteItem(serviceJid, nodeId, id);
	}

	@Override
	public Element getItem(String id) throws RepositoryException {
		return this.dao.getItem(serviceJid, nodeId, id);
	}

	@Override
	public Date getItemCreationDate(String id) throws RepositoryException {
		return this.dao.getItemCreationDate(serviceJid, nodeId, id);
	}

	@Override
	public String[] getItemsIds() throws RepositoryException {
		return this.dao.getItemsIds(serviceJid, nodeId);
	}

	@Override
	public String[] getItemsIdsSince(Date since) throws RepositoryException {
		return this.dao.getItemsIdsSince(serviceJid, nodeId, since);
	}

	@Override
	public List<ItemMeta> getItemsMeta() throws RepositoryException {
		return this.dao.getItemsMeta(serviceJid, nodeId, nodeName);
	}

	@Override
	public Date getItemUpdateDate(String id) throws RepositoryException {
		return this.dao.getItemUpdateDate(serviceJid, nodeId, id);
	}

	@Override
	public void writeItem(long timeInMilis, String id, String publisher, Element item) throws RepositoryException {
		this.dao.writeItem(serviceJid, nodeId, timeInMilis, id, publisher, item);
	}

}
