/*
 * Tigase Jabber/XMPP Publish Subscribe Component
 * Copyright (C) 2007 "Bartosz M. Małkowski" <bartosz.malkowski@tigase.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. Look for COPYING file in the top folder.
 * If not, see http://www.gnu.org/licenses/.
 *
 * $Rev$
 * Last modified by $Author$
 * $Date$
 */
package tigase.pubsub.repository;

import java.lang.reflect.Constructor;
import java.util.Date;
import java.util.Queue;
import java.util.logging.Level;
import java.util.logging.Logger;

import tigase.db.TigaseDBException;
import tigase.db.UserNotFoundException;
import tigase.db.UserRepository;
import tigase.form.Form;
import tigase.pubsub.AbstractNodeConfig;
import tigase.pubsub.CollectionNodeConfig;
import tigase.pubsub.LeafNodeConfig;
import tigase.pubsub.NodeType;
import tigase.pubsub.PubSubConfig;
import tigase.xml.DomBuilderHandler;
import tigase.xml.Element;
import tigase.xml.SimpleParser;
import tigase.xml.SingletonFactory;

public class PubSubDAO implements IPubSubDAO {

	public final static String CREATION_DATE_KEY = "creation-date";

	private static final String ITEMS_KEY = "items";

	public static final String NODES_KEY = "nodes/";

	private static final String ROOT_COLLECTION_KEY = "root-collection";

	final PubSubConfig config;

	private Logger log = Logger.getLogger(this.getClass().getName());

	private final SimpleParser parser = SingletonFactory.getParserInstance();

	final UserRepository repository;

	public PubSubDAO(UserRepository repository, PubSubConfig pubSubConfig)  {
		this.repository = repository;
		this.config = pubSubConfig;
	}

	@Override
	public void addListener(PubSubRepositoryListener listener) {
		throw new RuntimeException("Listeners are unsupported");
	}

	public void addToRootCollection(String nodeName) throws RepositoryException {
		try {
			repository.setData(config.getServiceName(), ROOT_COLLECTION_KEY, nodeName, "root");
		} catch (Exception e) {
			throw new RepositoryException("Adding to root collection error", e);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * tigase.pubsub.repository.PubSubRepository#createNode(java.lang.String,
	 * java.lang.String, tigase.pubsub.LeafNodeConfig, tigase.pubsub.NodeType,
	 * java.lang.String)
	 */
	public void createNode(String nodeName, String ownerJid, AbstractNodeConfig nodeConfig, NodeType nodeType, String collection)
			throws RepositoryException {
		try {
			nodeConfig.setNodeType(nodeType);
			repository.setData(config.getServiceName(), NODES_KEY + nodeName, CREATION_DATE_KEY,
					String.valueOf(System.currentTimeMillis()));

			if (nodeConfig != null)
				update(nodeName, nodeConfig);

		} catch (Exception e) {
			e.printStackTrace();
			throw new RepositoryException("Node creation error", e);
		}

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * tigase.pubsub.repository.PubSubRepository#deleteItem(java.lang.String,
	 * java.lang.String)
	 */
	public void deleteItem(String nodeName, String id) throws RepositoryException {
		try {
			repository.removeSubnode(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id);
		} catch (Exception e) {
			throw new RepositoryException("Item removing error", e);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * tigase.pubsub.repository.PubSubRepository#deleteNode(java.lang.String)
	 */
	public void deleteNode(String nodeName) throws RepositoryException {
		try {
			repository.removeSubnode(config.getServiceName(), NODES_KEY + nodeName);
		} catch (Exception e) {
			throw new RepositoryException("Node deleting error", e);
		}
	}

	@Override
	public String[] getBuddyGroups(String owner, String buddy) throws RepositoryException {
		try {
			return this.repository.getDataList(owner, "roster/" + buddy, "groups");
		} catch (Exception e) {
			throw new RepositoryException("Getting buddy groups error", e);
		}
	}

	@Override
	public String getBuddySubscription(String owner, String buddy) throws RepositoryException {
		try {
			return this.repository.getData(owner, "roster/" + buddy, "subscription");
		} catch (Exception e) {
			throw new RepositoryException("Getting buddy subscription status error", e);
		}
	}

	@Override
	public Element getItem(String nodeName, String id) throws RepositoryException {
		try {
			String itemData = repository.getData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "data");
			char[] data = itemData.toCharArray();
			DomBuilderHandler domHandler = new DomBuilderHandler();
			parser.parse(domHandler, data, 0, data.length);
			Queue<Element> q = domHandler.getParsedElements();
			return q.element();
		} catch (Exception e) {
			throw new RepositoryException("Item reading error", e);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * tigase.pubsub.repository.PubSubRepository#getItemCreationDate(java.lang
	 * .String, java.lang.String)
	 */
	public Date getItemCreationDate(final String nodeName, final String id) throws RepositoryException {
		try {
			String tmp = repository.getData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id,
					"creation-date");
			if (tmp == null)
				return null;
			Date d = new Date(Long.parseLong(tmp));
			return d;
		} catch (Exception e) {
			throw new RepositoryException("Items creation-date reading error", e);
		}
	}

	public String getItemPublisher(String nodeName, String id) throws RepositoryException {
		try {
			return repository.getData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "publisher");
		} catch (Exception e) {
			throw new RepositoryException("Items publisher reading error", e);
		}
	}

	@Override
	public String[] getItemsIds(String nodeName) throws RepositoryException {
		try {
			String[] ids = repository.getSubnodes(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY);
			// repository.getKeys(config.getServiceName(), NODES_KEY + nodeName
			// + "/" + ITEMS_KEY + "/");
			return ids;
		} catch (Exception e) {
			throw new RepositoryException("Items list reading error", e);
		}
	}

	public Date getItemUpdateDate(String nodeName, String id) throws RepositoryException {
		try {
			String tmp = repository.getData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id,
					"update-date");
			if (tmp == null)
				return null;
			Date d = new Date(Long.parseLong(tmp));
			return d;
		} catch (Exception e) {
			throw new RepositoryException("Items update-date reading error", e);
		}
	}

	@Override
	public NodeAffiliations getNodeAffiliations(String nodeName) throws RepositoryException {
		try {
			String cnfData = repository.getData(config.getServiceName(), NODES_KEY + nodeName, "affiliations");
			return NodeAffiliations.create(cnfData);
		} catch (Exception e) {
			throw new RepositoryException("Node subscribers reading error", e);
		}
	}

	public <T extends AbstractNodeConfig> T getNodeConfig(final Class<T> nodeConfigClass, final String nodeName,
			final Form configForm) throws RepositoryException {
		try {
			Constructor<T> constructor = nodeConfigClass.getConstructor(String.class);
			T nodeConfig = constructor.newInstance(nodeName);
			nodeConfig.copyFromForm(configForm);
			return nodeConfig;
		} catch (Exception e) {
			throw new RepositoryException("Node configuration reading error", e);
		}

	}

	public AbstractNodeConfig getNodeConfig(final String nodeName) throws RepositoryException {
		try {
			Form cnfForm = readNodeConfigForm(nodeName);
			if (cnfForm == null)
				return null;
			NodeType type = NodeType.valueOf(cnfForm.getAsString("pubsub#node_type"));
			Class<? extends AbstractNodeConfig> cl = null;
			switch (type) {
			case collection:
				cl = CollectionNodeConfig.class;
				break;
			case leaf:
				cl = LeafNodeConfig.class;
				break;
			default:
				throw new RepositoryException("Unknown node type " + type);
			}
			AbstractNodeConfig nc = getNodeConfig(cl, nodeName, cnfForm);
			return nc;
		} catch (RepositoryException e) {
			throw e;
		} catch (Exception e) {
			throw new RepositoryException("Node configuration reading error", e);
		}

	}

	public Date getNodeCreationDate(String nodeName) throws RepositoryException {
		try {
			String tmp = this.repository.getData(config.getServiceName(), NODES_KEY + nodeName, CREATION_DATE_KEY);
			long l = Long.parseLong(tmp);
			return new Date(l);
		} catch (Exception e) {
			throw new RepositoryException("Node creation date getting error", e);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see tigase.pubsub.repository.PubSubRepository#getNodesList()
	 */
	public String[] getNodesList() throws RepositoryException {
		try {
			String[] nodes;
			log.finer("Getting nodes list directly from DB");
			nodes = repository.getSubnodes(config.getServiceName(), NODES_KEY);
			return nodes;
		} catch (Exception e) {
			log.log(Level.WARNING, "Nodes list getting error", e);
			throw new RepositoryException("Nodes list getting error", e);
		}
	}

	@Override
	public NodeSubscriptions getNodeSubscriptions(String nodeName) throws RepositoryException {
		try {
			String cnfData = repository.getData(config.getServiceName(), NODES_KEY + nodeName, "subscriptions");
			return NodeSubscriptions.create(cnfData);
		} catch (Exception e) {
			throw new RepositoryException("Node subscribers reading error", e);
		}
	}

	public String[] getRootNodes() throws RepositoryException {
		try {
			String[] ids = repository.getKeys(config.getServiceName(), ROOT_COLLECTION_KEY);
			return ids;
		} catch (Exception e) {
			throw new RepositoryException("Getting root collection error", e);
		}
	}

	@Override
	public String[] getUserRoster(String owner) throws RepositoryException {
		try {
			return this.repository.getSubnodes(owner, "roster");
		} catch (Exception e) {
			throw new RepositoryException("Getting user roster error", e);
		}
	}

	@Override
	public void init() throws RepositoryException {
		try {
			this.repository.setData(this.config.getServiceName(), "last-start", String.valueOf(System.currentTimeMillis()));
		} catch (UserNotFoundException e) {
			try {
				this.repository.addUser(this.config.getServiceName());
				this.repository.setData(this.config.getServiceName(), "last-start", String.valueOf(System.currentTimeMillis()));
			} catch (Exception e1) {
				log.log(Level.SEVERE, "PubSub repository initialization problem", e1);
				throw new RepositoryException("Cannot initialize PubSUb repository", e);
			}
		} catch (TigaseDBException e) {
			log.log(Level.SEVERE, "PubSub repository initialization problem", e);
			throw new RepositoryException("Cannot initialize PubSUb repository", e);
		}
	}

	private Form readNodeConfigForm(final String nodeName) throws UserNotFoundException, TigaseDBException {
		String cnfData = repository.getData(config.getServiceName(), NODES_KEY + nodeName, "configuration");
		if (cnfData == null)
			return null;
		char[] data = cnfData.toCharArray();
		DomBuilderHandler domHandler = new DomBuilderHandler();
		parser.parse(domHandler, data, 0, data.length);
		Queue<Element> q = domHandler.getParsedElements();

		if (q != null && q.size() > 0) {
			Form form = new Form(q.element());
			return form;
		}
		return null;
	}

	public void removeAllFromRootCollection() throws RepositoryException {
		try {
			repository.removeSubnode(config.getServiceName(), ROOT_COLLECTION_KEY);
		} catch (Exception e) {
			throw new RepositoryException("Removing root collection error", e);
		}
	}

	public void removeFromRootCollection(String nodeName) throws RepositoryException {
		try {
			repository.removeData(config.getServiceName(), ROOT_COLLECTION_KEY, nodeName);
		} catch (Exception e) {
			throw new RepositoryException("Removing from root collection error", e);
		}
	}

	@Override
	public void removeListener(PubSubRepositoryListener listener) {
		throw new RuntimeException("Listeners are unsupported");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see tigase.pubsub.repository.PubSubRepository#update(java.lang.String,
	 * tigase.pubsub.LeafNodeConfig)
	 */
	public void update(final String nodeName, final AbstractNodeConfig nodeConfig) throws RepositoryException {
		try {
			String cnf = nodeConfig.getFormElement().toString();
			repository.setData(config.getServiceName(), NODES_KEY + nodeName, "configuration", cnf);
		} catch (Exception e) {
			throw new RepositoryException("Node configuration writing error", e);
		}

	}

	@Override
	public void update(String nodeName, IAffiliations affiliations) throws RepositoryException {
		try {
			String data = affiliations.serialize(true);
			repository.setData(config.getServiceName(), NODES_KEY + nodeName, "affiliations", data);
		} catch (Exception e) {
			throw new RepositoryException("Node subscribers writing error", e);
		}
	}

	@Override
	public void update(String nodeName, ISubscriptions subscriptions) throws RepositoryException {
		try {
			String data = subscriptions.serialize(true);
			repository.setData(config.getServiceName(), NODES_KEY + nodeName, "subscriptions", data);
		} catch (Exception e) {
			throw new RepositoryException("Node subscribers writing error", e);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * tigase.pubsub.repository.PubSubRepository#writeItem(java.lang.String,
	 * long, java.lang.String, java.lang.String, tigase.xml.Element)
	 */
	public void writeItem(final String nodeName, long timeInMilis, final String id, final String publisher, final Element item)
			throws RepositoryException {
		try {
			repository.setData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "data", item.toString());
			repository.setData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "creation-date",
					String.valueOf(timeInMilis));
			repository.setData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "update-date",
					String.valueOf(timeInMilis));
			repository.setData(config.getServiceName(), NODES_KEY + nodeName + "/" + ITEMS_KEY + "/" + id, "publisher", publisher);
		} catch (Exception e) {
			throw new RepositoryException("Item writing error", e);
		}
	}

}