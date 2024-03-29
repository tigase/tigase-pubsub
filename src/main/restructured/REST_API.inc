
REST API
-----------

All example calls to pubsub REST API are prepared for pubsub component running at ``pubsub.example.com``. It is required to replace this value with JID of pubsub component from your installation.

It is possible to provide parameters to requests as:

**XML**
   All parameters passed in content of HTTP request needs to be wrapped with ``<data/>`` tag as root tag of XML document, while returned parameters will be wrapped ``<result/>`` tag as root tag of XML document.

**JSON**
   Parameters must be passed as serialized JSON object. Additionally ``Content-Type`` header of HTTP request needs to be set to ``application/json``.

Create a node
^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/create-node``

Available HTTP methods:

GET
~~~~

Method returns example content which contains all required and optional parameters that may be passed to newly created node.

POST
~~~~~

Command requires fields ``node`` and ``pubsub#node_type`` to be filled with proper values for execution.

-  ``node`` - field should contain id of node to create

-  ``owner`` - field may contains jid which should be used as jid of owner of newly created node (will use jid of Tigase HTTP API Component if not passed)

-  ``pubsub#node_type`` - should contain type of node to create (two values are possible: ``leaf`` - node to which items will be published, ``collection`` - node which will contain other nodes)

Example content to create node of id ``example`` and of type ``leaf`` and with owner set to ``admin@example.com``.

Using XML
~~~~~~~~~~~

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <owner>admin@example.com</owner>
     <pubsub prefix="true">
       <node_type>leaf</node_type>
     </pubsub>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>

Using JSON
~~~~~~~~~~~

**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "owner" : "admin@example.com",
     "pubsub#node_type" : "leaf"
   }

**Response in JSON.**

.. code:: json

   {
     "Note": "Operation successful"
   }

Delete a node
^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/delete-node``

Available HTTP methods:


GET
~~~~~

Method returns example content which contains all required and optional parameters that may be passed.


POST
~~~~~

Command requires field ``node`` to be filled.

-  ``node`` - field should contain id of node to delete

Example content to delete node with id ``example``

Using XML
'''''''''''''

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>

Using JSON
''''''''''''

**Request in JSON.**

.. code:: json

   {
     "node" : "example"
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }


Subscribe to a node
^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/subscribe-node``

Available HTTP methods:

GET
~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~~

Command requires fields ``node`` and ``jids`` to be filled.

-  ``node`` - field should contain id of node to subscribe to

-  ``jids`` - field should contain list of jids to be subscribed to node

Example content to subscribe to node with id ``example`` users with jid ``test1@example.com`` and ``test2@example.com``


Using XML
'''''''''''

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <jids>
       <value>test1@example.com</value>
       <value>test2@example.com</value>
     </jids>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>


Using JSON

**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "jids" : [
       "test1@example.com",
       "test2@example.com"
     ]
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }


Unsubscribe from a node
^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/unsubscribe-node``

Available HTTP methods:

GET
~~~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~

Command requires fields ``node`` and ``jids`` to be filled.

-  ``node`` - field should contain id of node to unsubscribe from

-  ``jids`` - field should contain list of jids to be unsubscribed from node

Example content to unsubscribe from node with id ``example`` users ``test1@example.com`` and ``test2@example.com``

Using XML
~~~~~~~~~~

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <jids>
       <value>test@example.com</value>
       <value>test2@example.com</value>
     </jids>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>


Using JSON
~~~~~~~~~~~~

**Request in JSON.**

.. code:: json

   {
     "node" : "example.com",
     "jids" : [
       "test@example.com",
       "test2@example.com"
     ]
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }


Publish an item to a node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/publish-item``

Available HTTP methods:

GET
~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~

Command requires fields ``node`` and ``entry`` to be filled

-  ``node`` - field should contain id of node to publish to

-  ``item-id`` - field may contain id of entry to publish

-  ``expire-at`` - field may contain timestamp (in `XEP-0082 <http://xmpp.org/extensions/xep-0082.html>`__ format) after which item should not be delivered to user

-  ``entry`` - field should contain multi-line entry content which should be valid XML value for an item

Example content to publish item with id ``item-1`` to node with id ``example`` and with content in example field. P

Using XML
'''''''''''''

with XML payload

In this example we will use following XML payload:

**Payload.**

.. code:: xml

   <item-entry>
     <title>Example 1</title>
     <content>Example content</content>
   </item-entry>

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <item-id>item-1</item-id>
     <expire-at>2015-05-13T16:05:00+02:00</expire-at>
     <entry>
       <item-entry>
         <title>Example 1</title>
         <content>Example content</content>
       </item-entry>
     </entry>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>

with JSON payload

It is possible to publish JSON payload as value of published XML element. In example below we are publishing following JSON object:

**Payload.**

.. code:: json

   { "key-1" : 2, "key-2" : "value-2" }

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <item-id>item-1</item-id>
     <expire-at>2015-05-13T16:05:00+02:00</expire-at>
     <entry>
       <payload>{ &quot;key-1&quot; : 2, &quot;key-2&quot; : &quot;value-2&quot; }</payload>
     </entry>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>


Using JSON
'''''''''''

with XML payload

To publish XML using JSON you need to set serialized XML payload as value for ``entry`` key. In this example we will use following XML payload:

**Payload.**

.. code:: xml

   <item-entry>
     <title>Example 1</title>
     <content>Example content</content>
   </item-entry>

**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "item-id" : "item-1",
     "expire-at" : "2015-05-13T16:05:00+02:00",
     "entry" : "<item-entry>
       <title>Example 1</title>
       <content>Example content</content>
     </item-entry>"
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }


with JSON payload

As JSON needs to be set as a value of an XML element it will be wrapped on server side as a value for ``<payload/>`` element.

**Payload.**

.. code:: json

   { "key-1" : 2, "key-2" : "value-2" }

**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "item-id" : "item-1",
     "expire-at" : "2015-05-13T16:05:00+02:00",
     "entry" : {
       "key-1" : 2,
       "key-2" : "value-2"
     }
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }

**Published item.**

.. code:: xml

   <payload>{ &quot;key-1&quot; : 2, &quot;key-2&quot; : &quot;value-2&quot; }</payload>


Delete an item from a node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/delete-item``

Available HTTP methods:

GET
~~~~~
Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~

Command requires fields ``node`` and ``item-id`` to be filled

-  ``node`` - field contains id of node to publish to

-  ``item-id`` - field contains id of entry to publish

Example content to delete an item with id ``item-1`` from node with id ``example``.

Using XML
'''''''''''''
**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <item-id>item-1</item-id>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <Note type="fixed">
       <value>Operation successful</value>
     </Note>
   </result>


Using JSON
''''''''''''''
**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "item-id" : "item-1"
   }

**Response in JSON.**

.. code:: json

   {
     "Note" : "Operation successful"
   }


List available nodes
^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/list-nodes``

Available HTTP methods:

GET
~~~~~

Method returns list of available pubsub nodes for domain passed as part of URI (``pubsub.example.com``).

**Example response in XML.**

.. code:: xml

   <result>
     <title>List of available nodes</title>
     <nodes label="Nodes" type="text-multi">
       <value>test</value>
       <value>node_54idf40037</value>
       <value>node_3ws5lz0037</value>
     </nodes>
   </result>

in which we see nodes: ``test``, ``node_54idf40037`` and ``node_3ws5lz0037``.

**Example response in JSON.**

.. code:: json

   {
     "title" : "List of available nodes",
     "nodes" : [
       "test",
       "node_54idf40037",
       "node_3ws5lz0037"
     ]
   }

in which we see nodes: ``test``, ``node_54idf40037`` and ``node_3ws5lz0037``.

List published items on node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/list-items``

Available HTTP methods:


GET
~~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~~~~

Command requires field ``node`` to be filled

-  ``node`` - field contains id of node which items we want to list

Example content to list of items published on node with id ``example``.

Using XML
'''''''''''''

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <title>List of PubSub node items</title>
     <node label="Node" type="text-single">
       <value>example</value>
     </node>
     <items label="Items" type="text-multi">
       <value>item-1</value>
       <value>item-2</value>
     </items>
   </result>

where ``item-1`` and ``item-2`` are identifiers of published items for node ``example``.

Using JSON
''''''''''''''
**Request in JSON.**

.. code:: json

   {
     "node" : "example"
   }

**Response in JSON.**

.. code:: json

   {
     "title" : "List of PubSub node items",
     "node" : "example",
     "items" : [
       "item-1",
       "item-2"
     ]
   }

where ``item-1`` and ``item-2`` are identifiers of published items for node ``example``.

Retrieve item published on node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/retrieve-item``

Available HTTP methods:

GET
~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~~

Command requires fields ``node`` and ``item-id`` to be filled

-  ``node`` - field contains id of node which items we want to list

-  ``item-id`` - field contains id of item to retrieve

Example content to list of items published on node with id ``example``.

Using XML
'''''''''''

**Request in XML.**

.. code:: xml

   <data>
     <node>example</node>
     <item-id>item-1</item>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <title>Retrieve PubSub node item</title>
     <node label="Node" type="text-single">
       <value>example</value>
     </node>
     <item-id label="Item ID" type="text-single">
       <value>item-1</value>
     </item-id>
     <item label="Item" type="text-multi">
       <value>
         <item expire-at="2015-05-13T14:05:00Z" id="item-1">
           <item-entry>
             <title>Example 1</title>
             <content>Example content</content>
           </item-entry>
         </item>
       </value>
     </item>
   </result>

inside item element there is XML encoded element which is published on node ``example`` with id ``item-1``.

Using JSON
''''''''''''''''

**Request in JSON.**

.. code:: json

   {
     "node" : "example",
     "item-id" : "item-1"
   }

**Response in JSON.**

.. code:: json

   {
     "title" : "Retrieve PubSub node item",
     "node" : "example",
     "item-id" : "item-1",
     "item" : [
       "<item expire-at\"2015-05-13T14:05:00Z\" id=\"item-1\">
         <item-entry>
           <title>Example 1</title>
           <content>Example content</content>
         </item-entry>
       </item>"
     ]
   }


Retrieve user subscriptions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HTTP URI: ``/rest/pubsub/pubsub.example.com/retrieve-user-subscriptions``

Available HTTP methods:

GET
~~~~

Method returns example content which contains all required and optional parameters that may be passed.

POST
~~~~

Command requires field ``jid`` to be filled.

-  ``jid`` - field contains JID of a user for which we want to retrieve subscriptions

-  ``node-pattern`` - field contains regex pattern to match. When field is not empty, request will return only subscribed nodes which match this pattern. If field should be empty it may be omitted in a request.

Example content to retrieve list of nodes to which user ``test@example.com`` is subscribed at ``pubsub.example.com`` which starts with ``test-`` (pattern ``test-.*``)

Using XML
''''''''''''

**Request in XML.**

.. code:: xml

   <data>
     <jid>test@example.com</jid>
     <node-pattern>test-.*</node-pattern>
   </data>

**Response in XML.**

.. code:: xml

   <result>
     <nodes label="Nodes" type="text-multi">
       <value>test-123</value>
       <value>test-342</value>
     </nodes>
   </result>

Using JSON
'''''''''''''

**Request in JSON.**

.. code:: json

   {
     "jid" : "test@example.com",
     "node-pattern" : "test-.*"
   }

**Response in JSON.**

.. code:: json

   {
     "nodes" : [
       "test-123",
       "test-342"
     ]
   }
