/*
 * NodeSubscriptions.java
 *
 * Tigase Jabber/XMPP Publish Subscribe Component
 * Copyright (C) 2004-2017 "Tigase, Inc." <office@tigase.com>
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

package tigase.pubsub.repository.cached;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import tigase.pubsub.Subscription;
import tigase.pubsub.Utils;
import tigase.pubsub.repository.stateless.UsersSubscription;
import tigase.xmpp.jid.BareJID;

public class NodeSubscriptions extends tigase.pubsub.repository.NodeSubscriptions {

	protected final ThreadLocal<Map<BareJID, UsersSubscription>> changedSubs = new ThreadLocal<Map<BareJID, UsersSubscription>>();

	public NodeSubscriptions() {
	}

	/**
	 * Constructs ...
	 *
	 *
	 * @param nodeSubscriptions
	 */
	public NodeSubscriptions(tigase.pubsub.repository.NodeSubscriptions nodeSubscriptions) {
		subs.putAll(nodeSubscriptions.getSubscriptionsMap());
	}

	/**
	 * Method description
	 *
	 *
	 * @param jid
	 * @param subscription
	 *
	 * @return
	 */
	@Override
	public String addSubscriberJid(BareJID bareJid, Subscription subscription) {
		final String subid = Utils.createUID(bareJid);
		UsersSubscription s = new UsersSubscription(bareJid, subid, subscription);

		changedSubs().put(bareJid, s);

		return subid;
	}

	private Map<BareJID, UsersSubscription> changedSubs() {
		Map<BareJID, UsersSubscription> changedSubs = this.changedSubs.get();
		if (changedSubs == null) {
			changedSubs = new HashMap<BareJID, UsersSubscription>();
			this.changedSubs.set(changedSubs);
		}
		return changedSubs;
	}

	/**
	 * Method description
	 *
	 *
	 * @param jid
	 * @param subscription
	 */
	@Override
	public void changeSubscription(BareJID bareJid, Subscription subscription) {
		UsersSubscription s = subs.get(bareJid);

		if (s != null) {
			s.setSubscription(subscription);

			changedSubs().put(s.getJid(), s);
		}
	}

	@Override
	protected UsersSubscription get(final BareJID bareJid) {
		UsersSubscription us = null;

		us = changedSubs().get(bareJid);

		if (us == null) {
			us = subs.get(bareJid);

			if (us != null) {
				try {
					return us.clone();
				} catch (Exception e) {
					e.printStackTrace();

					return null;
				}
			}
		}

		return us;
	}

	public Map<BareJID, UsersSubscription> getChanged() {
		return changedSubs();
	}

	/**
	 * Method description
	 *
	 *
	 * @return
	 */
	@Override
	public UsersSubscription[] getSubscriptions() {
		final Set<UsersSubscription> result = new HashSet<UsersSubscription>();

		result.addAll(this.subs.values());

		result.addAll(this.changedSubs().values());

		return result.toArray(new UsersSubscription[] {});
	}

	/**
	 * Method description
	 *
	 *
	 * @return
	 */
	@Override
	public boolean isChanged() {
		return this.changedSubs().size() > 0;
	}

	/**
	 * Method description
	 *
	 */
	public void merge() {
		Map<BareJID, UsersSubscription> changedSubs = changedSubs();
		for (Map.Entry<BareJID, UsersSubscription> entry : changedSubs.entrySet()) {
			if (entry.getValue().getSubscription() == Subscription.none) {
				subs.remove(entry.getKey());
			} else {
				subs.put(entry.getKey(), entry.getValue());
			}
		}
		// subs.putAll(changedSubs);
		changedSubs.clear();
	}

	/**
	 * Method description
	 *
	 */
	@Override
	public void resetChangedFlag() {
		changedSubs().clear();
	}
}
