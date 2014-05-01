<?php
namespace MangoPay;

/**
 * Base class for MangoPay API managers
 */
abstract class ApiBase {
    
    /**
     * Root/parent instance that holds the OAuthToken and Configuration instance
     * @var \MangoPay\MangoPayApi
     */
    protected $_root;
    
    /**
     * Array with REST url and request type
     * @var array 
     */
    private $_methods = array(
        'authentication_base' => array( '/api/clients/', RequestType::POST ),
        'authentication_oauth' => array( '/api/oauth/token', RequestType::POST ),
        
        'crosscurrencytransfers_create' => array( '/transfers/%s', RequestType::POST ),
        'crosscurrencytransfers_get' => array( '/transfers/%s', RequestType::GET ),
        
        'events_all' => array( '/events', RequestType::GET ),
        'events_gethookcallbacks' => array( '/events/%s/hook-callbacks', RequestType::GET ),
        
        'hooks_create' => array( '/hooks', RequestType::POST ),
        'hooks_all' => array( '/hooks', RequestType::GET ),
        'hooks_get' => array( '/hooks/%s', RequestType::GET ),
        'hooks_save' => array( '/hooks/%s', RequestType::PUT ),
        
        'info_get' => array( '/info', RequestType::GET ),
        'info_getfeewallets' => array( '/info/fee-wallets', RequestType::GET ),
        'info_getmeansofpayment' => array( '/info/means-of-payment', RequestType::GET ),
     
        'cardregistration_create' => array( '/cardregistrations', RequestType::POST ),
        'cardregistration_get' => array( '/cardregistrations/%s', RequestType::GET ),
        'cardregistration_save' => array( '/cardregistrations/%s', RequestType::PUT ),
        
        'card_get' => array( '/cards/%s', RequestType::GET ),
        
        // pay ins URLs
        'payins_card-web_create' => array( '/payins/card/web/', RequestType::POST ),
        'payins_card-direct_create' => array( '/payins/card/direct/', RequestType::POST ),
        'payins_card-preauthorized_create' => array( '/payins/card/preauthorized/', RequestType::POST ),
        'payins_card-recurrentexecution_create' => array( '/payins/card/recurrent-pay-in-execution/', RequestType::POST ),
        
        'payins_registeredcard-web_create' => array( '/payins/card/web/', RequestType::POST ),
        'payins_registeredcard-direct_create' => array( '/payins/card/direct/', RequestType::POST ),
        'payins_registeredcard-preauthorized_create' => array( '/payins/registered-card/preauthorized/', RequestType::POST ),
        'payins_registeredcard-recurrentexecution_create' => array( '/payins/registered-card/recurrent-pay-in-execution/', RequestType::POST ),
        
        'payins_bankwirepayin-web_create' => array( '/payins/bankwire/web/', RequestType::POST ),
        'payins_bankwirepayin-direct_create' => array( '/payins/bankwire/direct/', RequestType::POST ),
        'payins_bankwirepayin-preauthorized_create' => array( '/payins/bankwire/preauthorized/', RequestType::POST ),
        'payins_bankwirepayin-recurrentexecution_create' => array( '/payins/bankwire/recurrent-pay-in-execution/', RequestType::POST ),
        
        'payins_directcredit-web_create' => array( '/payins/direct-credit/web/', RequestType::POST ),
        'payins_directcredit-direct_create' => array( '/payins/direct-credit/direct/', RequestType::POST ),
        'payins_directcredit-preauthorized_create' => array( '/payins/direct-credit/preauthorized/', RequestType::POST ),
        'payins_directcredit-recurrentexecution_create' => array( '/payins/direct-credit/recurrent-pay-in-execution/', RequestType::POST ),
        'payins_get' => array( '/payins/%s', RequestType::GET ),
        'payins_getrefunds' => array( '/payins/%s/refunds', RequestType::GET ),
        'payins_createrefunds' => array( '/payins/%s/refunds', RequestType::POST ),
        
        'payouts_bankwire_create' => array( '/payouts/bankwire/', RequestType::POST ),
        'payouts_merchantexpense_create' => array( '/payouts/merchant-expense/', RequestType::POST ),
        'payouts_amazongiftcard_create' => array( '/payouts/amazon-giftcard/', RequestType::POST ),
        'payouts_get' => array( '/payouts/%s', RequestType::GET ),
        'payouts_createrefunds' => array( '/payouts/%s/refunds', RequestType::POST ),
        'payouts_getrefunds' => array( '/payouts/%s/refunds', RequestType::GET ),
        
        'reccurringpayinorders_create' => array( '/reccurring-pay-in-orders', RequestType::POST ),
        'reccurringpayinorders_get' => array( '/reccurring-pay-in-orders/%s', RequestType::GET ),
        'reccurringpayinorders_gettransactions' => array( '/reccurring-pay-in-orders/%s/transactions', RequestType::GET ),
        
        'refunds_get' => array( '/refunds/%s', RequestType::GET ),
        
        'repudiations_get' => array( '/repudiations/%s', RequestType::GET ),
        
        'transfers_create' => array( '/transfers', RequestType::POST ),
        'transfers_get' => array( '/transfers/%s', RequestType::GET ),
        'transfers_getrefunds' => array( '/transfers/%s/refunds', RequestType::GET ),
        'transfers_createrefunds' => array( '/transfers/%s/refunds', RequestType::POST ),
        
        'users_createnaturals' => array( '/users/natural', RequestType::POST ),
        'users_createlegals' => array( '/users/legal', RequestType::POST ),
        'users_createkycrequest' => array( '/users/%s/KYC/requests', RequestType::POST ),
        'users_createbankaccounts' => array( '/users/%s/bankaccounts', RequestType::POST ),
        'users_all' => array( '/users', RequestType::GET ),
        'users_allkyc' => array( '/users/%s/KYC', RequestType::GET ),
        'users_allkycrequests' => array( '/users/%s/KYC/requests', RequestType::GET ),
        'users_allwallets' => array( '/users/%s/wallets', RequestType::GET ),
        'users_allbankaccount' => array( '/users/%s/bankaccounts', RequestType::GET ),
        'users_allpaymentcards' => array( '/users/%s/payment-cards', RequestType::GET ),
        'users_get' => array( '/users/%s', RequestType::GET ),
        'users_getnaturals' => array( '/users/natural/%s', RequestType::GET ),
        'users_getlegals' => array( '/users/legal/%s', RequestType::GET ),
        'users_getkycrequest' => array( '/users/%s/KYC/requests/%s', RequestType::GET ),
        'users_getproofofidentity' => array( '/users/%s/ProofOfIdentity', RequestType::GET ),
        'users_getproofofaddress' => array( '/users/%s/ProofOfAddress', RequestType::GET ),
        'users_getproofofregistration' => array( '/users/%s/ProofOfRegistration', RequestType::GET ),
        'users_getshareholderdeclaration' => array( '/users/%s/ShareholderDeclaration', RequestType::GET ),
        'users_getbankaccount' => array( '/users/%s/bankaccounts/%s', RequestType::GET ),
        'users_getpaymentcard' => array( '/users/%s/payment-cards/%s', RequestType::GET ),
        'users_savenaturals' => array( '/users/natural/%s', RequestType::PUT ),
        'users_savelegals' => array( '/users/legal/%s', RequestType::PUT ),
        
        'wallets_create' => array( '/wallets', RequestType::POST ),
        'wallets_allrecurringpayinorders' => array( '/wallets/%s/recurring-pay-in-orders', RequestType::GET ),
        'wallets_alltransactions' => array( '/wallets/%s/transactions', RequestType::GET ),
        'wallets_get' => array( '/wallets/%s', RequestType::GET ),
        'wallets_save' => array( '/wallets/%s', RequestType::PUT ),
        
        'kyc_documents_create' => array( '/users/%s/KYC/documents/', RequestType::POST ),
        'kyc_documents_get' => array( '/users/%s/KYC/documents/%s', RequestType::GET ),
        'kyc_documents_save' => array( '/users/%s/KYC/documents/%s', RequestType::PUT ),
        'kyc_page_create' => array( '/users/%s/KYC/documents/%s/pages', RequestType::POST ),
    );

    /**
     * Constructor
     * @param \MangoPay\MangoPayApi Root/parent instance that holds the OAuthToken and Configuration instance
     */
    function __construct($root) {
        $this->_root = $root;
    }
    
    /**
     * Get URL for REST Mango Pay API
     * @param string $key Key with data
     * @return string 
     */
    protected function GetRequestUrl($key){
        return $this->_methods[$key][0];
    }
    
    /**
     * Get request type for REST Mango Pay API
     * @param string $key Key with data
     * @return RequestType 
     */
    protected function GetRequestType($key){
        return $this->_methods[$key][1];
    }
    
    /**
     * Create object in API
     * @param string $methodKey Key with request data
     * @param object $entity Entity object
     * @param object $responseClassName Name of entity class from response
     * @param int $entityId Entity identifier
     * @return object Respons data
     */
    protected function CreateObject($methodKey, $entity, $responseClassName = null, $entityId = null, $subEntityId = null) {

        if (is_null($entityId))
            $urlMethod = $this->GetRequestUrl($methodKey);
        elseif (is_null($subEntityId))
            $urlMethod = sprintf($this->GetRequestUrl($methodKey), $entityId);
        else
            $urlMethod = sprintf($this->GetRequestUrl($methodKey), $entityId, $subEntityId);
        
        $requestData = null;
        if (!is_null($entity))
            $requestData = $this->BuildRequestData($entity);

        $rest = new RestTool(true, $this->_root);

        $response = $rest->Request($urlMethod, $this->GetRequestType($methodKey), $requestData);
      
        if (!is_null($responseClassName))
            return $this->CastResponseToEntity($response, $responseClassName);
        
        return $response;
    }
    
    /**
     * Get entity object from API
     * @param string $methodKey Key with request data
     * @param int $entityId Entity identifier
     * @param object $responseClassName Name of entity class from response
     * @param int $secondEntityId Entity identifier for second entity
     * @return object Respons data
     */
    protected function GetObject($methodKey, $entityId, $responseClassName = null, $secondEntityId = null) {
        
        $urlMethod = sprintf($this->GetRequestUrl($methodKey), $entityId, $secondEntityId);
        
        $rest = new RestTool(true, $this->_root);
        $response = $rest->Request($urlMethod, $this->GetRequestType($methodKey));
        
        if (!is_null($responseClassName))
            return $this->CastResponseToEntity($response, $responseClassName);
        
        return $response;
    }
    
    /**
     * Get lst with entities object from API
     * @param string $methodKey Key with request data
     * @param \MangoPay\Pagination $pagination Pagination object
     * @param object $responseClassName Name of entity class from response
     * @param int $entityId Entity identifier
     * @param object $filter Object to filter data
     * @return object Respons data
     */
    protected function GetList($methodKey, & $pagination, $responseClassName = null, $entityId = null, $filter = null) {
        
        $urlMethod = sprintf($this->GetRequestUrl($methodKey), $entityId);
        
        if (is_null($pagination) || !is_object($pagination) || get_class($pagination) != 'MangoPay\Pagination') {
            $pagination = new Pagination();
        }
        
        $rest = new RestTool(true, $this->_root);
        $response = $rest->Request($urlMethod, $this->GetRequestType($methodKey), null, $pagination, $filter);
        
        if (!is_null($responseClassName))
            return $this->CastResponseToEntity($response, $responseClassName);
        
        return $response;
    }
    
    /**
     * Save object in API
     * @param string $methodKey Key with request data
     * @param object $entity Entity object to save
     * @param object $responseClassName Name of entity class from response
     * @return object Respons data
     */
    protected function SaveObject($methodKey, $entity, $responseClassName = null, $secondEntityId = null) {
        
        if (is_null($secondEntityId))
            $urlMethod = sprintf($this->GetRequestUrl($methodKey), $entity->Id);
        else
            $urlMethod = sprintf($this->GetRequestUrl($methodKey), $secondEntityId, $entity->Id);

        $requestData = $this->BuildRequestData($entity);
        
        $rest = new RestTool(true, $this->_root);
        $response = $rest->Request($urlMethod, $this->GetRequestType($methodKey), $requestData);

        if (!is_null($responseClassName))
            return $this->CastResponseToEntity($response, $responseClassName);
        
        return $response;
    }
    
    /**
     * Cast response object to entity object
     * @param object $response Object from API response
     * @param string $entityClassName Name of entity class to cast
     * @return \MangoPay\$entityClassName Return entity object
     */
    protected function CastResponseToEntity($response, $entityClassName, $asDependentObject = false)
    {
        if (is_array($response)) {
            
            $list = array();
            foreach ($response as $reponseObject) {
                array_push($list, $this->CastResponseToEntity($reponseObject, $entityClassName));
            }
            
            return $list;
        }
        
        if (is_string($entityClassName)) {
            $entity = new $entityClassName();
        } else {
            throw new Exception('Cannot cast response to entity object. Wrong entity class name');
        }
        
        $responseReflection = new \ReflectionObject($response);
        $entityReflection = new \ReflectionObject($entity);
        $responseProperties = $responseReflection->getProperties();
        
        $subObjects = $entity->GetSubObjects();
        $dependsObjects = $entity->GetDependsObjects();

        foreach ($responseProperties as $responseProperty) {
            
            $responseProperty->setAccessible(true);
            
            $name = $responseProperty->getName();
            $value = $responseProperty->getValue($response);
            
            if ($entityReflection->hasProperty($name)) {
                
                $entityProperty = $entityReflection->getProperty($name);
                $entityProperty->setAccessible(true);
                
                // is sub object?
                if (isset($subObjects[$name])) {
                    $object = $this->CastResponseToEntity($value, $subObjects[$name]);
                    $entityProperty->setValue($entity, $object);
                } else {
                    $entityProperty->setValue($entity, $value);
                }

                // has dependent object?
                if (isset($dependsObjects[$name])) {
                    $dependsObject = $dependsObjects[$name];
                    $entityDependProperty = $entityReflection->getProperty($dependsObject['_property_name']);
                    $entityDependProperty->setAccessible(true);
                    $entityDependProperty->setValue($entity, $this->CastResponseToEntity($response, $dependsObject[$value], true));
                }
            } else {
                if ($asDependentObject || !empty($dependsObjects)) {
                    continue;
                }
                else {
					/* UNCOMMENT THE LINE BELOW TO ENABLE RESTRICTIVE REFLECTION MODE */
					//throw new Exception('Cannot cast response to entity object. Missing property ' . $name .' in entity ' . $entityClassName);
					
					continue;
				}
            }
        }
        
        return $entity;
    }
    
    /**
     * Get array with request data
     * @param object $entity Entity object to send as request data
     * @return array 
     */
    protected function BuildRequestData($entity) {
        
        $entityProperies = get_object_vars($entity);
        $blackList = $entity->GetReadOnlyProperties();
        $requestData = array();
        foreach ($entityProperies as $propertyName => $propertyValue) {
            
            if (in_array($propertyName, $blackList))
                continue;
        
            if ($this->CanReadSubRequestData($entity, $propertyName)) {
                $subRequestData = $this->BuildRequestData($propertyValue);
                foreach ($subRequestData as $key => $value) {
                    $requestData[$key] = $value;
                }
            } else {
                if (isset($propertyValue))
                    $requestData[$propertyName] = $propertyValue;
            }
        }
        
        return $requestData;
    }
    
    private function CanReadSubRequestData($entity, $propertyName) {
        if (get_class($entity) == 'MangoPay\PayIn' && 
                    ($propertyName == 'PaymentDetails' || $propertyName == 'ExecutionDetails')) {
            return true;
        }
        
        if (get_class($entity) == 'MangoPay\PayOut' && $propertyName == 'MeanOfPaymentDetails') {
            return true;
        }
        
        return false;
    }
}
