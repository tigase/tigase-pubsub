Announcement
~~~~~~~~~~~~~~~~~

Major changes
''''''''''''''''

Tigase pubsub component has undergone a few major changes to our code and structure. To continue to use Tigase pubsub component, a few changes may be needed to be made to your systems. Please see them below:

Database schema changes

Current version comes with changes to database schema to improve JID comparison during lookup of nodes, subscriptions, affiliations, etc.

To continue usage of new versions of pubsub component it is required to manually load new component database schema, see `database preparation <#databasePreparation>`__ section for more information.

.. Warning::

    Loading of new database schema is required to use new version of pubsub component.

Changes in REST API

We continuously work on improving usability and making our REST API easier to use we added support for handling JSON requests in REST API for pubsub. At the same time we decided to slightly modify responses in XML sent by REST API to make responses in JSON and XML similar.

For more informations about current REST API please look into `Rest API <#restAPI>`__ section.

New features
'''''''''''''
Support for using separate database for different domains

Since this version it is possible to use separate pubsub nodes and items based on domains. This allows you to configure component to store informations about nodes and items for particular domain to different database.

For more informations please look into `using multiple databases <#multidb>`__.

Support for MAM

In this version we added support for `XEP-0313: Message Archive Management <http://xmpp.org/extensions/xep-0313.html:>`__ protocol which allows any MAM compatible XMPP client with pubsub support to retrieve items published on pubsub nodes using MAM protocol for querying.