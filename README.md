MangoPay Wordpress Plugin Web (MWPW)
==================

EasyDigitalDownloads Mangopay gateway (94.7% compatible with Campaignify)

Mangopay SDKv2 [implementing **web payment method**].

Workflow
----
Once the plugin is configured, **use it as any other EDD payment gateway**. Notice that the gateway won't appear on *wp-admin|Campaigns|Settings|Gateways|Payment Gateways* check box list until it is fully configured.

By design, will use global *$edd_options* to store site wide admin and wallet. This is because multiple campaigns could be filling the cart and only one redirection to validation card web server is done, so **payment will go to 'site wallet' and then distributed to 'campaigns wallets'**. This is also designed to implement, in the future, 'all or nothing' compatibility with Crowdfunding plugin. Once you set a *WP_user* as site admin, initialization procedure will attempt to generate a wallet. Get the id's as:
- $edd_options['mwpw_mangopay_wallets_owner_id']
- $edd_options['mwpw_site_admin_wp_id']
- $edd_options['mwpw_mangopay_site_wallet_id']

Also, notice that **every created wallet will be owned by site admin**, although every campaign stores its owner and wallet. This is to avoid loosing data on changing campaigns author. However, 'wallet class' constructor is waiting for you to override this.

**PayOut will be withdrawed to campaign's author**, once *wp-admin|Profile|MangoPay User|MangoPay bank account for payout* section is filled. **See *MangoPay fundraisings* metabox on campaigns edit page** page where you will find a button to order withdraw. This could be easily removed if 'all_or_nothing' featured is implemented. Remember that MangoPay staff will process manually de bank transfer. Once a payout is requested, this section will be blocked until it is commited.

Then, *WP_users*, and *WP_posts* objects will be completed with fields as follows:

a) **Site admin** WP_user:

 - *mangopay_id* (int)  MangoPay\User (legal or natural) id => It will be generated on user *Profile|MangoPay User*.
 - *wallet_id*   (int)  MangoPay\Wallet id                  => It will be generated on plugin initialization; (see *mwpw_init_site()* function in *mwpw_gateway.php* file)

b) **Post author** WP_user:

 - *mangopay_id* (int)	 MangoPay\User (legal or natural) id  => It will be generated on user *Profile|MangoPay User*.
 - *bank_id*	    (int)	 MangoPay\BankAccount id              => It will be generated on user *Profile|MangoPay                                                                       User|MangoPay bank account for payout*.

c) **Campaign** type='download' WP_post:

 - *wallet_id*	  (int)	 MangoPay\Wallet id                   => It will be generated on first payment to wallet; (see *mwpw_distribute_funds()* function on *mwpw_pay* class)


Also notice that **Wordpress guests** are allowed to pay without registration. In this cases, a *MangoPay\User* will be generated but won't be linked to any *WP_user*. 

As Mangopay SDKv2 doesn't implements deleting *MangoPay\User* or *MangoPay\bankAccounts*, every time a *WP_user* changes 'user type' from *natural* to *legal* or viceversa, a new MangoPay id will be generated. Same way, if bank updates.


Configuration
--------------
Set gateway default values on *Campaings|Settings|Gateways|Mangopay Gateway Setting*:

* Site Admin => Select a site administrator. You will able to check here, current configured id's. And also current wallet balance. This value should be always 0, but this may increase if any loosed funds have been credited but not distributed to corresponding campaign wallet. An EDD payment note should exists with reported errors during process.
* Live API User / Live API Key => this is generated by Mangopay *client_creation.php*. [Get info ](http://docs.mangopay.com/api-references/start-in-production/).
* Temp folder => This is MangopaySDKv2 requested temp folder.
* Conditions rules image =>  url where you have uploaded MangoPay logo (MangoPay contract requirement)
* Conditions & rules =>  url where you have uploaded MangoPay conditions ((MangoPay contract requirement)

Notices
-------------
In case you get **Unauthorized** errors, check MangoPaySDK/types/configuration.inc (search for public $BaseUrl) to set proper environment. Also, if you have been moving from sandbox, try erasing MangoPay tmp file (you provided a temporal folder for MangoPay to write).

Version
----
0.3.14159265359 [SemVer](http://semver.org/)

Tech
-----------
*Mangopay_edd_plugin_web* uses a number of open source projects to work properly:
* Wordpress.org - https://github.com/WordPress/WordPress
* Easy-Digital-Downloads - https://github.com/easydigitaldownloads/Easy-Digital-Downloads
* Astoundify / crowdfunding - https://github.com/Astoundify/crowdfunding

Dependencies
--------------
- [Mangopay SDKv2](https://github.com/MangoPay/mangopay2-php-sdk)

##### Calaways EDD gateways bundle.

Docs dev
-------------
* [Wiki doc](https://wiki.enredaos.net/index.php?title=COOPFUND-DEV#MANGOPAY).


Status
--------------
I've got a non uploaded to github gateway working on www.coopfunding.net (this is spanish web site). So you can see it working.

For now, this gateway is working perfectly. But it has several issues that must be solved and present gateway solves them:

Critical features:
- Only first item in cart is processed.
- A issue related to user that owns wallets. Here, post (campaign) author owns the wallet. But if for some reason author changes his user type in his profile from Natural to Legal or viceversa, then a new MangoPay id is generated so will loose relation with wallet and then an admin should readjust it by code.

Non-critical features:
- Fundify 'all or nothing' funds retrievement not implemented. So any payment will go directly to campaigns wallet.
- https://github.com/aleph1888/mangopay_edd_plugin/issues
- Non English traduction available (need to translate from spain).

Just for beauty features:
- Code not documented
- There is some uggly code related to paymentDIRECT (in-site credit card validation) needed to erase. As now it is implementing paymentWEB (redirection credit card validation), this code must be removed just to clean. This is because I used previous gateway version to built the new one.

Then, uploaded gateway fixes all these features except:

Non-critical features:
- Fundify 'all or nothing' funds retrievement not implemented. So any payment will go directly to campaigns wallet.
Still not hooked Fundify trigger, but now gateway is receiving all funds to a site wallet and then distributing it to posts (campaigns) wallets, so, it is only one final step by easily intercepting distribution process.
- Non English traduction available (need to translate from spanis)

Then, the problem is that this one, has not been tested already. Even not installed, so may be it has even some sintaxis errors. I need to install it, and verify everything goes fine. Just a few hours.

License
----------
GPL

Contribute
----------
@BTC 1DNxbBeExzv7JvXgL6Up5BSUvuY4gE8q4

Other ways: ox@enredaos.net



