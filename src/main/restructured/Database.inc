Database
----------

Preparation of database
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before you will be able to use Tigase PubSub Component you need to initialize database. We provide few schemas for this component for MySQL, PostgreSQL, SQLServer and DerbyDB.

They are placed in ``database/`` directory of installation package and named in ``dbtype-pubsub-version.sql``, where ``dbname`` in name of database type which this schema supports and ``version`` is version of a PubSub component for which this schema is designed.

You need to manually select schema for correct database and component and load this schema to database. For more information about loading database schema look into `database preperation <#databasePreperation>`__ section of this guide.

Upgrade of database schema
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Database schema for our components may change between versions and if so it needs to be updated before new version may be started. To upgrade schema please follow instructions from the `database preperation <#databasePreperation>`__ section.

.. Note::

   If you use SNAPSHOT builds then schema may change for same version as this are versions we are still working on.

Schema description
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tigase PubSub component uses few tables and stored procedures. To make it easier to identify tables and stored procedures used by PubSub component they are prefixed with ``tig_pubsub_``.

Table ``tig_pubsub_service_jids``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This table stores all jids for which PubSub component contains nodes.

+------------------+--------------------------------------+----------------------------------------------------+
| Field            | Description                          | Comments                                           |
+==================+======================================+====================================================+
| service_id       | Database ID of a service JID         |                                                    |
+------------------+--------------------------------------+----------------------------------------------------+
| service_jid      | Value of a service JID               |                                                    |
+------------------+--------------------------------------+----------------------------------------------------+
| service_jid_sha1 | SHA1 value of lowercased service JID | Used for proper bare JID comparison during lookup. |
|                  |                                      |                                                    |
|                  |                                      | (N/A to PostgreSQL schema)                         |
+------------------+--------------------------------------+----------------------------------------------------+

Table ``tig_pubsub_jids``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This table stores all jids related to PubSub nodes, ie. subscriber, affiliates, creators, publishers, etc.

+----------+-----------------------------------+----------------------------------------------------+
| Field    | Description                       | Comments                                           |
+==========+===================================+====================================================+
| jid_id   | Database ID of a bare JID         |                                                    |
+----------+-----------------------------------+----------------------------------------------------+
| jid      | Value of a bare JID               |                                                    |
+----------+-----------------------------------+----------------------------------------------------+
| jid_sha1 | SHA1 value of lowercased bare JID | Used for proper bare JID comparison during lookup. |
|          |                                   |                                                    |
|          |                                   | (N/A to PostgreSQL schema)                         |
+----------+-----------------------------------+----------------------------------------------------+

Table ``tig_pubsub_nodes``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Table stores nodes tree structure and node configuration.

+---------------+-----------------------------------------+------------------------------------------------------------+
| Field         | Description                             | Comments                                                   |
+===============+=========================================+============================================================+
| node_id       | Database ID of a node                   |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| service_id    | ID of service JID                       | References ``service_id`` from ``tig_pubsub_service_jids`` |
+---------------+-----------------------------------------+------------------------------------------------------------+
| name          | Name of PubSub node                     |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| name_sha1     | SHA1 of PubSub node name                | Used for indexing and faster lookup.                       |
|               |                                         |                                                            |
|               |                                         | (N/A to PostgreSQL schema)                                 |
+---------------+-----------------------------------------+------------------------------------------------------------+
| type          | Type of PubSub node                     | 0 - collection                                             |
|               |                                         |                                                            |
|               |                                         | 1 - leaf                                                   |
+---------------+-----------------------------------------+------------------------------------------------------------+
| title         | Title of PubSub node                    |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| description   | Description of a node                   |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| creator_id    | ID of JID of creator                    | References ``jid_id`` from ``tig_pubsub_jids``             |
+---------------+-----------------------------------------+------------------------------------------------------------+
| creation_date | Timestamp of creation time              |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| configuration | Serialized configuration of PubSub node |                                                            |
+---------------+-----------------------------------------+------------------------------------------------------------+
| collection_id | Points collection (parent node)         | References ``node_id`` from ``tig_pubsub_nodes``           |
+---------------+-----------------------------------------+------------------------------------------------------------+

Table ``tig_pubsub_affiliations``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Table stores affiliations between PubSub nodes and jids.

+-------------+-------------------+--------------------------------------------------+
| Field       | Description       | Comments                                         |
+=============+===================+==================================================+
| node_id     | ID of a node      | References ``node_id`` from ``tig_pubsub_nodes`` |
+-------------+-------------------+--------------------------------------------------+
| jid_id      | ID of a user JID  | References ``jid_id`` from ``tig_pubsub_jids``   |
+-------------+-------------------+--------------------------------------------------+
| affiliation | Affiliation value |                                                  |
+-------------+-------------------+--------------------------------------------------+

Table ``tig_pubsub_subscriptions``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Table stores subscriptions of jids to PubSub nodes.

+-----------------+----------------------+--------------------------------------------------+
| Field           | Description          | Comments                                         |
+=================+======================+==================================================+
| node_id         | ID of a node         | References ``node_id`` from ``tig_pubsub_nodes`` |
+-----------------+----------------------+--------------------------------------------------+
| jid_id          | ID of a user JID     | References ``jid_id`` from ``tig_pubsub_jids``   |
+-----------------+----------------------+--------------------------------------------------+
| subscription    | Subscription value   |                                                  |
+-----------------+----------------------+--------------------------------------------------+
| subscription_id | Id of a subscription |                                                  |
+-----------------+----------------------+--------------------------------------------------+

Table ``tig_pubsub_items``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Table stores items of PubSub nodes.

+---------------+-------------------------------------+--------------------------------------------------+
| Field         | Description                         | Comments                                         |
+---------------+-------------------------------------+--------------------------------------------------+
| node_id       | ID of a node                        | References ``node_id`` from ``tig_pubsub_nodes`` |
+---------------+-------------------------------------+--------------------------------------------------+
| id            | Id of an items                      |                                                  |
+---------------+-------------------------------------+--------------------------------------------------+
| id_sha1       | SHA1 of item id                     | Indexed and used for faster lookup               |
|               |                                     |                                                  |
|               |                                     | (N/A to PostgreSQL schema)                       |
+---------------+-------------------------------------+--------------------------------------------------+
| creation_date | Creation date                       |                                                  |
+---------------+-------------------------------------+--------------------------------------------------+
| publisher_id  | ID of publisher JID                 | References ``jid_id`` from ``tig_pubsub_jids``   |
+---------------+-------------------------------------+--------------------------------------------------+
| update_date   | Timestamp of last item modification |                                                  |
+---------------+-------------------------------------+--------------------------------------------------+
| data          | Item payload                        |                                                  |
+---------------+-------------------------------------+--------------------------------------------------+
