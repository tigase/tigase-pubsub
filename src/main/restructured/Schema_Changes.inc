PubSub Schema Changes
^^^^^^^^^^^^^^^^^^^^^^^^

Tigase PubSub Component is currently version 3.3.0 which is introduced in Tigase server v8.0.0.

PubSub 3.2.0 Changes
~~~~~~~~~~~~~~~~~~~~~~~

PubSub v 3.2.0 adds a new procedure TigPubSubGetNodeMeta which supports PubSub metadata retrieval while conducting a disco#info query on nodes.

You will need to upgrade your database if you are not using v3.2.0 schema. Tigase will report being unable to load PubSub component if you do not have this schema version.

The MySQL schema can be found `Here <https://projects.tigase.org/projects/tigase-pubsub/repository/revisions/master/entry/database/mysql-pubsub-schema-3.2.0.sql>`__.

The Derby schema can be found `Here <https://projects.tigase.org/projects/tigase-pubsub/repository/changes/database/derby-pubsub-schema-3.2.0.sql>`__.

The PostGRESQL schema can be found `Here <https://projects.tigase.org/projects/tigase-pubsub/repository/changes/database/postgresql-pubsub-schema-3.2.0.sql>`__.

The MS SQL schema can be found `Here <https://projects.tigase.org/projects/tigase-pubsub/repository/changes/database/sqlserver-pubsub-schema-3.2.0.sql>`__.

The same files are also included in all distributions of v8.0.0 in ``[tigaseroot]/database/`` . All changes to database schema are meant to be backward compatible.

For instructions how to manually upgrade the databases, please refer to `Tigase v7.1.0 Schema Updates section <#tigaseServer71>`__.

Upgrading older installations (pre-v3.0.0 Schema)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To update older installations of Tigase to the PubSub Schema v3.0.0 follow these instructions. Note this should be done before upgrading to PubSub v3.1.0.

Step by Step guide.

Prepare Old Database for Upgrade
'''''''''''''''''''''''''''''''''''

In ``database`` directory of Tigase installation you will find SQL files which will prepare old database schema for upgrade using following this naming pattern: ``<database_type>-pubsub-schema-3.0.0-pre-upgrade.sql`` Where ``<database_type>`` can be one of the following: ``mysql``, ``sqlserver``, ie. for MySQL you will find the file ``mysql-pubsub-schema-3.0.0-pre-upgrade.sql``. You need to execute statements from this file on your source database, which will drop old procedures and functions used to access database and also this statements will rename old tables by adding suffix \_1 to each of old tables. Example:

**MySQL**
   ``mysql -u tigase -p tigase_pubsub < database/mysql-pubsub-schema-3.0.0-pre-upgrade.sql``

**MS SQL**
   ``sqlcmd -S %servername% -U %root_user% -P %root_pass% -d %database% -i database\sqlserver-pubsub-schema-3.0.0-pre-upgrade.sql``

Update Tigase PubSub Component
''''''''''''''''''''''''''''''''

For this you need to copy the Tigase PubSub Component jar file to jars directory inside Tigase XMPP Server installation directory. It is also recommended to copy files from database directory of Tigase PubSub Component to database directory in Tigase XMPP Server installation directory.

If you happen to use one of the the distribution packaged (either installer or -dist-max flavored archive) then all required files are already available - both new schema files will be available in ``database/`` directory as well as both versions of PubSub component will be present in ``jars/`` directory - PubSub3 as tigase-pubsub.jar and PubSub2 as tigase-pubsub-2.2.0.jar.old (provided for compatibility reasons).

Load New Schema
''''''''''''''''

In the ``database`` directory you will find files containing new schemas for:

-  MySQL - ``mysql-pubsub-schema-3.0.0.sql``

-  PostgreSQL - ``postgresql-pubsub-schema-3.0.0.sql``

-  MSSQL - ``sqlserver-pubsub-schema-3.0.0.sql``

-  DerbyDB - ``derby-pubsub-schema-3.0.0.sql`` and ``pubsub-db-create-derby.sh``

For most databases, with the exception of Derby, you only need to execute statements from the proper file. For example:

**MySQL**
   ``mysql -u tigase -p tigase_pubsub < database/mysql-pubsub-schema-3.0.0.sql``

**MS SQL**
   ``sqlcmd -S %servername% -U %root_user% -P %root_pass% -d %database% -i database\sqlserver-pubsub-schema-3.0.0.sql``

**PostgreSQL**
   ``psql -h $DB_HOST -q -U ${USR_NAME} -d $DB_NAME -f database/sqlserver-pubsub-schema-3.0.0.sql``

For DerbyDB you need to execute the ``pubsub-db-create-derby.sh`` script and pass proper JDBC URI to database to which you want to load schema (if database does not exist, it will be created).

::

   database/pubsub-db-create-derby.sh

**NOTE:** It is possible to use same database which was used before - then after upgrade you will have new tables and old tables with \_1 suffix.

Execute Migration Utility
'''''''''''''''''''''''''''

In the ``/database`` directory you will find the ``pubsub-db-migrate.sh`` file which you need to execute and pass arguments with JDBC URIs needed to connect to source and destination database. If you used dedicated tables for PubSub you will also need to pass a class name used to access database (value of ``pubsub-repo-class`` variable from ``etc/config.tdsl`` file).

Example for dedicated table used for PubSub:

.. code:: sql

   database/pubsub-db-migrate.sh -in-repo-class tigase.pubsub.repository.PubSubDAO
   -in 'jdbc:mysql://localhost/tigase_pubsub?user=tigase&password=passwd'
   -out 'jdbc:mysql://localhost/tigase_pubsub?user=tigase&password=passwd'

Example for use without dedicated PubSub tables:

.. code:: sql

   database/pubsub-db-migrate.sh
   -in 'jdbc:mysql://localhost/tigase?user=tigase&password=passwd'
   -out 'jdbc:mysql://localhost/tigase?user=tigase&password=passwd'

Example for use with dedicated tables in a Windows environment:

.. code:: sql

   database/pubsub-db-migrate.cmd -in-repo-class tigase.pubsub.repository.PubSubDAO
   -in 'jdbc:sqlserver://<hostname>\\<instance>:<port>;databaseName=<name>;user=tigase;password=tigase;schema=dbo;lastUpdateCount=false'
   -out 'jdbc:sqlserver://<hostname>\\<instance>:<port>;databaseName=<name>;user=tigase;password=tigase;schema=dbo;lastUpdateCount=false'

During execution this utility will report information about migration of PubSub data to the new schema, and the same information will be store in ``pubsub_db_migration.log``.

Finish
'''''''

After successful migration you will have all data copied to new tables. Old tables will be renamed by adding suffix \_1. After verification that everything works OK, you can delete old tables and it’s content as it want be used any more.