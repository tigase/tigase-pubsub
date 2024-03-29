Configuration
--------------

Pubsub naming
^^^^^^^^^^^^^^^^^^

Within Tigase, all pubsub component address MUST be domain-based address and not a JID style address. This was made to simplify communications structure. Tigase will automatically set component names to pubsub.domain, however any messages send to pubsub@domain will result in a ``SERVICE_UNAVAILABLE`` error.

Pubsub nodes within Tigase can be found as a combination of JID and node where nodes will be identified akin to service discovery. For example, to address a friendly node, use the following structure:

.. code:: xml

   <iq to='pubsub.domain'>
     <query node='friendly node'/>
   </iq>


Configure Roster Maximum size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Administrators can configure the maximum allowable roster size per user via the config.tdsl file.

.. code:: text

   'sess-man' {
       'jabber:iqa:roster' {
           max_roster_size = '100'
       }
   }

This sets the roster limit to 100 entries per user. It can be set to any integer, however by default no limit is set and no configuration is set in ``config.tdsl`` file.

Store Full XML of Last Presence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tigase can store a more detailed ``<unavailable/>`` presence stanza to include timestamps and other information.

Requirements
~~~~~~~~~~~~~

Ensure that ``presence-offline`` plugin is enabled in config.tdsl. To do this, add be sure ``presence-offline`` is listed under ``sess-man``

.. code:: text

   'sess-man' {
       'presence-offline' () {}
   }

The following two lines in ``sess-man`` configure options to broadcast probes to offline users.

.. code:: text

   'sess-man' {
       'skip-offline' = 'false'
       'skip-offline-sys' = 'false'
   }

Without these lines, Tigase will not send presence probes to users that the server knows to be offline.

The full XML presence is stored under the tig_pairs table with a pkey of ``last-unavailable-presence`` will look like this:

.. code:: xml

   <presence from="user@example.com" xmlns="jabber:client" type="unavailable">
   <status>Logged out</status>
   <delay stamp="2015-12-29T16:51:50.748Z" xmlns="urn:xmpp:delay"/></presence>

As you can see, the plugin has added a delay stamp which indicates the last time they were seen online. This may be suppressed by using the following line in your config.tdsl file.

.. code:: text

   'sess-man' {
       'delay-stamp' = 'false'
   }

You may also limit probe responses only to newly connected resources.

.. code:: text

   'sess-man' {
       'probe-full-jid' = 'true'
   }

When a user logs on, they will receive the same full unavailable presence statements from contacts not logged in. Also the repository entry containing their last unavailable presence will be removed.

**NOTE: This will increase traffic with users with many people on their rosters.**

Using separate store
^^^^^^^^^^^^^^^^^^^^^

As mentioned above, by default Tigase pubsub component uses default data source configured for Tigase XMPP Server. It is possible to use separate store by pubsub component. To do so you need to configure new ``DataSource`` in ``dataSource`` section. Here we will use ``pubsub-store`` as name of newly configured data source. Additionally you need to pass name of newly configured data source to ``dataSourceName`` property of default DAO of pubsub component.

::

   dataSource {
       pubsub-store () {
           uri = 'jdbc:postgresql://server/pubsub-database'
       }
   }

   pubsub () {
       dao {
           default () {
               dataSourceName = 'pubsub-store'
           }
       }
   }

It is also possible to configure separate store for particular domain, ie. ``pubsub.example.com``. Here we will configure data source with name ``pubsub.example.com`` and use it to store data for pubsub nodes and items at ``pubsub.example.com``:

::

   dataSource {
       'pubsub.example.com' () {
           uri = 'jdbc:postgresql://server/example-database'
       }
   }

   pubsub () {
       dao {
           'pubsub.example.com' () {
             # we may not set dataSourceName as it matches name of domain
           }
       }
   }

.. Note::

   With this configuration, data for other domains than ``pubsub.example.com`` will be stored in default data source.


Enabling PEP support
^^^^^^^^^^^^^^^^^^^^^^^^

To enable `XEP-0163: Personal Eventing Protocol <http://xmpp.org/extensions/xep-0163.html>`__ support it is required to set ``persistent-pep`` property of pubsub component to ``true``, set ``send-last-published-item-on-presence`` property of component to ``true`` and enable ``pep`` SessionManager processor.

::

   pubsub () {
       persistent-pep = true
       send-last-published-item-on-presence = true
   }

   sess-man () {
       pep () {
       }
   }

.. Note::

   If your pubsub component uses different name than ``pubsub`` then you need to set ``pubsub-jid`` property of ``pep`` processor to JID of pubsub component make it aware of a different name of a pubsub component.

**Example with pubsub component named ``events`` hosted at server named ``servername.com`` and enabled PEP.**

::

   events () {
       persistent-pep = true
       send-last-published-item-on-presence = true
   }
   sess-man () {
       pep () {
           'pubsub-jid' = 'events@servername.com'
       }
   }


Enabling REST API
^^^^^^^^^^^^^^^^^^^^^^

To use REST API for pubsub component it is required that:

-  Tigase HTTP API component is installed and configured properly. For information about HTTP API component installation please look into `HTTP component documentation <#compHTTPAPI>`__.

-  Tigase pubsub REST scripts are copied to HTTP API REST scripts directory In installation package this is already done and scripts are in proper locations. dd\* JID of HTTP API component needs to be added to list of trusted jids of Tigase pubsub component ``trusted`` property (if ``http`` is name of HTTP API component)

::

   pubsub () {
       trusted = [ 'http@{clusterNode}' ];
   }

Changing nodes cache size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default Tigase pubsub component caches node configuration of 2000 last loaded nodes. If there are many requests to database to load node configuration and your installation contains many nodes it may be a good idea to increase number of cached nodes.

To do this you need to set ``pubsub-repository-cache-size`` property of pubsub component to new size.

::

   pubsub () {
       pubsub-repository-cache-size = 4000
   }


Enable sending last published item on presence
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default it is not possible to use delivery of last published item when users broadcasts initial presence. To do so you need to set ``send-last-published-item-on-presence`` of pubsub component to ``true``. This will allow you to configure nodes to send last published item on presence.

::

   pubsub () {
       send-last-published-item-on-presence = true
   }

Throttling sending notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Notifications sent by PubSub component may be sent in large batches, if you have a nodes with a lot of subscribers. In those cases, it is useful to throttle publications to improve behaviour and performance of other parts of Tigase XMPP Server.

To achieve that, PubSub throttles generate notifications to specified throughput. By default it is set to 5k packets for each CPU core available per second.

To set it to a different value, you can set ``limit`` property of ``publishExecutor`` bean to the expected number of publications per second, ie. 100000;

.. Note::

   This value is a number of total throughput, and will not be adjusted by the number of available CPU cores.

::

   pubsub () {
       publishExecutor () {
           limit = 10000
       }
   }

Publication rate is also adjusted to current memory usage on a 4 point scale adjusted to the value of two configuration options: ``highMemoryUsageLimit`` and ``criticalMemoryUsageLimit`` (with default values: 90% and 98% respectively): \* ``normal`` - if memory usage is below ``highMemoryUsageLimit`` (i.e. below 90%) \* ``high`` - memory usage less than halfway between ``highMemoryUsageLimit`` and ``veryHigh`` (i.e. between 90% and 94%) \* ``veryHigh`` - memory usage more than halfway between ``highMemoryUsageLimit`` and ``veryHigh`` (i.e. between 94% and 98%) \* ``critical`` - if memory usage is above ``criticalMemoryUsageLimit`` (i.e. above 98%)

It’s possible to adjust values of the high and critical limits in publisher bean:

::

   pubsub () {
       publishExecutor () {
           highMemoryUsageLimit = 90
           criticalMemoryUsageLimit = 98
       }
   }


Disable automatic subscription of node creator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

During creation of node pubsub component subscribes creator to pubsub node and delivers notifications to creator. If in your case you do not want this behavior, you may set ``auto-subscribe-node-creator`` property of pubsub component to ``false``.

::

   pubsub () {
       auto-subscribe-node-creator = false
   }
