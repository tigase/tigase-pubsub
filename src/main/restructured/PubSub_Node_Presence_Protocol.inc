PubSub Node Presence Protocol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Occupant Use Case**

Log in to Pubsub Node
~~~~~~~~~~~~~~~~~~~~~~~

To log in to PubSub Node user must send presence to PubSub component with additional information about node:

.. code:: xml

   <presence
       from='hag66@shakespeare.lit/pda'
       id='n13mt3l'
       to='pubsub.shakespeare.lit'>
     <pubsub xmlns='tigase:pubsub:1' node='princely_musings'/>
   </presence>

Component will publish this information in node:

.. code:: xml

   <message from='pubsub.shakespeare.lit' to='francisco@denmark.lit' id='foo'>
     <event xmlns='http://jabber.org/protocol/pubsub#event'>
       <items node='princely_musings'>
         <item>
           <presence xmlns='tigase:pubsub:1' node='princely_musings' jid='hag66@shakespeare.lit/pda' type='available'/>
         </item>
       </items>
     </event>
   </message>
   <message from='pubsub.shakespeare.lit' to='bernardo@denmark.lit' id='bar'>
     <event xmlns='http://jabber.org/protocol/pubsub#event'>
       <items node='princely_musings'>
         <item>
           <presence xmlns='tigase:pubsub:1' node='princely_musings' jid='hag66@shakespeare.lit/pda' type='available'/>
         </item>
       </items>
     </event>
   </message>

And then will send notification with presences of all occupants to new occupant.

Log out from PubSub Node
~~~~~~~~~~~~~~~~~~~~~~~~~~

To logout from single node, user must send presence stanza with type unavailable:

.. code:: xml

   <presence
       from='hag66@shakespeare.lit/pda'
       type='unavailable'
       to='pubsub.shakespeare.lit'>
     <pubsub xmlns='tigase:pubsub:1' node='princely_musings'/>
   </presence>

Component will send events to all occupants as described:

.. code:: xml

   <message from='pubsub.shakespeare.lit' to='francisco@denmark.lit' id='foo'>
     <event xmlns='http://jabber.org/protocol/pubsub#event'>
       <items node='princely_musings'>
         <item>
           <presence xmlns='tigase:pubsub:1' node='princely_musings' jid='hag66@shakespeare.lit/pda' type='unavailable'/>
         </item>
       </items>
     </event>
   </message>

If component receives presence stanza with type unavailable without specified node, then component will log out user from all nodes he logged before and publish events.

Retrieving list of all Node Subscribers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To retrieve list of node subscribers, node configuration option ``tigase#allow_view_subscribers`` must be set to true:

.. code:: xml

   <iq type='set'
       from='hamlet@denmark.lit/elsinore'
       to='pubsub.shakespeare.lit'
       id='config2'>
     <pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>
       <configure node='princely_musings'>
         <x xmlns='jabber:x:data' type='submit'>
           <field var='FORM_TYPE' type='hidden'>
             <value>http://jabber.org/protocol/pubsub#node_config</value>
           </field>
           <field var='tigase#allow_view_subscribers'><value>1</value></field>
         </x>
       </configure>
     </pubsub>
   </iq>

When option is enabled, each subscriber may get list of subscribers the same way `as owner <http://xmpp.org/extensions/xep-0060.html#owner-subscriptions-retrieve>`__.

.. code:: xml

   <iq type='get'
       from='hamlet@denmark.lit/elsinore'
       to='pubsub.shakespeare.lit'
       id='subman1'>
     <pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>
       <subscriptions node='princely_musings'/>
     </pubsub>
   </iq>

There is extension to filter returned list:

.. code:: xml

   <iq type='get'
       from='hamlet@denmark.lit/elsinore'
       to='pubsub.shakespeare.lit'
       id='subman1'>
     <pubsub xmlns='http://jabber.org/protocol/pubsub#owner'>
       <subscriptions node='princely_musings'>
           <filter xmlns='tigase:pubsub:1'>
               <jid contains='@denmark.lit' />
           </filter>
       </subscriptions>
     </pubsub>
   </iq>

In this example will be returned all subscriptions of users from domain "denmark.lit".

