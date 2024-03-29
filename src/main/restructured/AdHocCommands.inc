AdHoc Commands
----------------

Similar to the HTTP API, AdHoc commands based on groovy scripts can be sent to this component to do a number of tasks. All scripts for these Ad-hoc commands are found at ``sec/main/groovy/tigase/admin`` in source distributions, or at `this link <https://projects.tigase.org/projects/tigase-pubsub/repository/revisions/master/show/src/main/groovy/tigase/admin>`__. To use them, the scripts need to be copied into the ``scripts/admin/pubsub`` folder in the Tigase installation directory. For all examples, the component address will be ``pubsub.example.com``.

Create a Node
^^^^^^^^^^^^^^^^^^

Ad-hoc command node: ``create-node`` Required role: Service Administrator

Command requires fields ``node`` and ``pubsub#node_type`` to be filled with proper values for execution. - ``node`` Field containing id of node to create. - ``pubsub#node_type`` Contains one of two possible values. \* ``leaf-node`` Node that will be published. \* ``collection`` Node that will contain other nodes.

Other fields are optional fields that can be set to change configuration of newly create node to different configuration than default.

Example call using TCLMT:

::

   bin/tclmt.sh -u admin@example.com -p admin123 remote pubsub.example.com create-node example admin@example.com leaf

Delete a Node
^^^^^^^^^^^^^^^^^

Ad-hoc command node: ``delete-node`` Required role: Service Administrator

Command requires ``node`` field to be filled. - ``node`` Field containing id of node to delete.

Example call using TCLMT:

::

   bin/tclmt.sh -u admin@example.com -p admin123 remote pubsub.example.com delete-node example

Subscribe to a Node
^^^^^^^^^^^^^^^^^^^^^^^

Ad-hoc command node: ``subscribe-node`` Required role: Service Administrator

Command requires ``node`` and ``jids`` nodes to be filled. - ``node`` Field containing node to subscribe to. - ``jids`` Field containing list of JIDs to subscribe to the node.

Example call using TCLMT:

::

   bin/tclmt.sh -u admin@example.com -p admin123 remote pubsub.example.com subscribe-node example admin@example.com,test1@example.com


Unsubscribe to a Node
^^^^^^^^^^^^^^^^^^^^^^^^^^

Ad-hoc command node: ``unsubscribe-node`` Required role: Service Administrator

Command requires ``node`` and ``jids`` nodes to be filled. - ``node`` Field containing node to unsubscribe to. - ``jids`` Field containing list of JIDs to unsubscribe to the node.

Example call using TCLMT:

::

   bin/tclmt.sh -u admin@example.com -p admin123 remote pubsub.example.com unsubscribe-node example admin@example.com,test2@example.com


Publish an item to a Node
^^^^^^^^^^^^^^^^^^^^^^^^^^

Ad-hoc command node: ``publish-item`` Required role: Service Administrator

Command requires fields ``node`` and ``entry`` to be filled. - ``node`` Field containing id of node to publish to. - ``item-id`` Field may contain id of entry to publish, can be empty. - ``entry`` Field should contain multi-line entry content that should be valid XML values for items.

This command due to it’s complexity cannot be easily executed by TCLMT using default remote script which provides support for basic adhoc commands. Example call using TCLMT:

::

   bin/tclmt.sh -u admin@example.com -p admin123 remote pubsub.example.com publish-item example item-1 '<entry><title>Example 1</title></entry>'

Example Groovy script to execute create-node command using JAXMPP2

.. code:: java

   import tigase.jaxmpp.j2se.Jaxmpp
   import tigase.jaxmpp.core.client.AsyncCallback
   import tigase.jaxmpp.core.client.exceptions.JaxmppException
   import tigase.jaxmpp.core.client.xmpp.stanzas.Stanza
   import tigase.jaxmpp.core.client.SessionObject
   import tigase.jaxmpp.j2se.ConnectionConfiguration
   import tigase.jaxmpp.core.client.xml.Element
   import tigase.jaxmpp.core.client.xml.DefaultElement
   import tigase.jaxmpp.core.client.xmpp.forms.JabberDataElement

   Jaxmpp jaxmpp = new Jaxmpp();

   jaxmpp.with {
       getConnectionConfiguration().setConnectionType(ConnectionConfiguration.ConnectionType.socket)
       getConnectionConfiguration().setUserJID("admin@example.com")
       getConnectionConfiguration().setUserPassword("admin123")
   }

   jaxmpp.login(true);

   def packet = IQ.create();
   packet.setAttribute("to", "pubsub.example.com");

   Element command = new DefaultElement("command");
   command.setXMLNS("http://jabber.org/protocol/commands");
   command.setAttribute("node", "create-node");
   packet.addChild(command);

   Element x = new DefaultElement("x");
   x.setXMLNS("jabber:x:data");

   command.addChild(x);

   def data = new JabberDataElement(x);
   data.addTextSingleField("node", "example");
   data.addListSingleField("pubsub#node_type", "leaf");

   jaxmpp.send(packet, new AsyncCallback() {
       void onError(Stanza responseStanza, tigase.jaxmpp.core.client.XMPPException.ErrorCondition error) throws JaxmppException {
           println "received error during processing request";
       }

       void onSuccess(Stanza responseStanza) throws JaxmppException {
           x = responseStanza.getFirstChild("command").getFirstChid("x");
           data = new JabberDataElement(x);
           def error = data.getField("Error");
           println "command executed with result = " + (error ? "failure, error = " + error.getFieldValue() : "success");
       }

       void onTimeout() {
           println "command timed out"
       }
   });

   Thread.sleep(30000);
   jaxmpp.disconnect();

