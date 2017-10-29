/*
 * SubscribeNode.groovy
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

/*
 Subscribe to PubSub node

 AS:Description: Subscribe to node
 AS:CommandId: subscribe-node
 AS:Component: pubsub
 */

package tigase.admin

import groovy.transform.CompileStatic
import tigase.db.TigaseDBException
import tigase.eventbus.EventBus
import tigase.kernel.core.Kernel
import tigase.pubsub.*
import tigase.pubsub.exceptions.PubSubException
import tigase.pubsub.repository.IPubSubRepository
import tigase.server.Command
import tigase.server.Iq
import tigase.server.Packet
import tigase.xml.Element
import tigase.xmpp.Authorization
import tigase.xmpp.jid.BareJID

Kernel kernel = (Kernel) kernel;
PubSubComponent component = (PubSubComponent) component
packet = (Iq) packet
eventBus = (EventBus) eventBus

@CompileStatic
Packet process(Kernel kernel, PubSubComponent component, Iq p, EventBus eventBus, Set admins) {
	def NODE = "node"
	def JIDS = "jids";

	def componentConfig = kernel.getInstance(PubSubConfig.class);
	IPubSubRepository pubsubRepository = kernel.getInstance(IPubSubRepository.class);

	def stanzaFromBare = p.getStanzaFrom().getBareJID()
	def isServiceAdmin = admins.contains(stanzaFromBare)

	def node = Command.getFieldValue(p, NODE)
	def jids = Command.getFieldValues(p, JIDS);

	if (node == null) {
		def result = p.commandResult(Command.DataType.form);

		Command.addTitle(result, "Subscribe to a node")
		Command.addInstructions(result, "Fill out this form to subscribe to a node.")

		Command.addFieldValue(result, NODE, node ?: "", "text-single",
							  "The node to subscribe to")
		Command.addFieldValue(result, JIDS, jids ? jids.join("\n") : "", "jid-multi",
							  "JIDs to subscribe")

		return result
	}

	def result = p.commandResult(Command.DataType.result)
	try {
		if (isServiceAdmin || componentConfig.isAdmin(stanzaFromBare)) {
			def toJid = p.getStanzaTo().getBareJID();

			AbstractNodeConfig nodeConfig = pubsubRepository.getNodeConfig(toJid, node);

			if (nodeConfig == null) {
				throw new PubSubException(Authorization.ITEM_NOT_FOUND, "Node " + node + " needs " +
						"to be created before anyone will be able to subscribe to a node.");
			}

			def nodeAffiliations = pubsubRepository.getNodeAffiliations(toJid, node);
			def nodeSubscriptions = pubsubRepository.getNodeSubscriptions(toJid, node);

			jids.each { String jidStr ->
				def jid = BareJID.bareJIDInstance(jidStr);
				def subscription = nodeSubscriptions.getSubscription(jid)
				def affiliation = nodeAffiliations.getSubscriberAffiliation(jid).getAffiliation();

				affiliation = affiliation.getWeight() > Affiliation.member.getWeight() ? affiliation :
							  Affiliation.member;
				String subid = nodeSubscriptions.getSubscriptionId(jid);

				if (subid == null) {
					subid = nodeSubscriptions.addSubscriberJid(jid, Subscription.subscribed);
					nodeAffiliations.addAffiliation(jid, affiliation);
				} else {
					nodeSubscriptions.changeSubscription(jid, Subscription.subscribed);
					nodeAffiliations.changeAffiliation(jid, affiliation);
				}

				if (nodeSubscriptions.isChanged()) {
					pubsubRepository.update(toJid, node, nodeSubscriptions);
				}
				if (nodeAffiliations.isChanged()) {
					pubsubRepository.update(toJid, node, nodeAffiliations);
				}

			}

			Command.addTextField(result, "Note", "Operation successful");
		} else {
			//Command.addTextField(result, "Error", "You do not have enough permissions to publish item to a node.");
			throw new PubSubException(Authorization.FORBIDDEN,
									  "You do not have enough " + "permissions to publish item to a node.");
		}
	} catch (PubSubException ex) {
		Command.addTextField(result, "Error", ex.getMessage())
		if (ex.getErrorCondition()) {
			def error = ex.getErrorCondition();
			Element errorEl = new Element("error");
			errorEl.setAttribute("type", error.getErrorType());
			Element conditionEl = new Element(error.getCondition(), ex.getMessage());
			conditionEl.setXMLNS(Packet.ERROR_NS);
			errorEl.addChild(conditionEl);
			Element pubsubCondition = ex.pubSubErrorCondition?.getElement();
            if (pubsubCondition) {
                errorEl.addChild(pubsubCondition)
            };
			result.getElement().addChild(errorEl);
		}
	} catch (TigaseDBException ex) {
		Command.addTextField(result, "Note", "Problem accessing database, not subscribed to node.");
	}
	return result;
}

return process(kernel, component, packet, eventBus, (Set) adminsSet)
