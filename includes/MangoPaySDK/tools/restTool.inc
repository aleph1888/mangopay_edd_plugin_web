<?php
namespace MangoPay;

/**
 * Class to prepare HTTP request, call the request and decode the response 
 */
class RestTool {

    /**
     * Root/parent instance that holds the OAuthToken and Configuration instance
     * @var \MangoPay\MangoPayApi
     */
    private $_root;

    /**
     * Variable to flag that in request authentication data are required
     * @var bool 
     */
    private $_authRequired;
    
    /**
     * Array with HTTP header to send with request
     * @var array 
     */
    private $_requestHttpHeaders;
    
    /**
     * cURL handle
     * @var resource  
     */
    private $_curlHandle;
    
    /**
     * Request type for current request
     * @var RequestType 
     */
    private $_requestType;
    
    /**
     * Array with data to pass in the request
     * @var array 
     */
    private $_requestData;
    
    /**
     * Code get from response
     * @var int 
     */
    private $_responseCode;
    
    /**
     * Pagination object
     * @var MangoPay\Pagination 
     */
    private $_pagination;
    
    private $_requestUrl;
    
    private static $_JSON_HEADER = 'Content-Type: application/json';
        
    /**
     * Constructor
     * @param bool $authRequired Variable to flag that in request the authentication data are required
     * @param \MangoPay\MangoPayApi Root/parent instance that holds the OAuthToken and Configuration instance
     */
    function __construct($authRequired = true, $root) {
        $this->_authRequired = $authRequired;
        $this->_root = $root;
    }
    
    public function AddRequestHttpHeader($httpHeader) {
        
        if (is_null($this->_requestHttpHeaders))
            $this->_requestHttpHeaders = array();
        
        array_push($this->_requestHttpHeaders, $httpHeader);
    }
    
    /**
     * Call request to MangoPay API
     * @param string $urlMethod Type of method in REST API
     * @param \MangoPay\RequestType $requestType Type of request
     * @param array $requestData Data to send in request
     * @param \MangoPay\Pagination $pagination Pagination object
     * @return object Respons data
     */
    public function Request($urlMethod, $requestType, $requestData = null, & $pagination = null, $additionalUrlParams = null) {
       
        $this->_requestType = $requestType;
        $this->_requestData = $requestData;
        
        $this->BuildRequest($urlMethod, $pagination, $additionalUrlParams);
         
        $responseResult = $this->RunRequest();
        
        if(!is_null($pagination)){
            $pagination = $this->_pagination;
        }
        
        return $responseResult;
    }
    
    /**
     * Execute request and check response
     * @return object Respons data
     * @throws Exception If cURL has error
     */
    private function RunRequest() {

        $result = curl_exec($this->_curlHandle);
        
        if ($result === false && curl_errno($this->_curlHandle) != 0)
            throw new Exception('cURL error: ' . curl_error($this->_curlHandle));

        $this->_responseCode = (int)curl_getinfo($this->_curlHandle, CURLINFO_HTTP_CODE);
        
        curl_close($this->_curlHandle);
                 
        if ($this->_root->Config->DebugMode) 
            Logs::Debug('Response JSON', $result);
        
        $response = json_decode($result); 
      
        if ($this->_root->Config->DebugMode) 
            Logs::Debug('Response object', $response);
        
        $this->CheckResponseCode($response);
        
        return $response;
    }
    
    /**
     * Prepare all paraemter to request
     * @param String $urlMethod Type of method in REST API
     * @throws Exception If some parameters are not set
     */
    private function BuildRequest($urlMethod, $pagination, $additionalUrlParams = null) {
        
        $urlTool = new UrlTool($this->_root);
        $restUrl = $urlTool->GetRestUrl($urlMethod, $this->_authRequired, $pagination, $additionalUrlParams);

        $this->_requestUrl = $urlTool->GetFullUrl($restUrl);
        if ($this->_root->Config->DebugMode) 
            Logs::Debug('FullUrl', $this->_requestUrl);
        
        $this->_curlHandle = curl_init($this->_requestUrl);
        if ($this->_curlHandle === false)
            throw new Exception('Cannot initialize cURL session');
        
        curl_setopt($this->_curlHandle, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($this->_curlHandle, CURLOPT_SSL_VERIFYPEER, false);
        if (!is_null($pagination)) {
            curl_setopt($this->_curlHandle, CURLOPT_HEADERFUNCTION, array(&$this, 'ReadResponseHeader'));
            $this->_pagination = $pagination;
        }
        
        switch ($this->_requestType) {
            case RequestType::POST:
                curl_setopt($this->_curlHandle, CURLOPT_POST, true);
                break;
            case RequestType::PUT:
                curl_setopt($this->_curlHandle, CURLOPT_CUSTOMREQUEST, 'PUT');
                break;
            case RequestType::DELETE:
                curl_setopt($this->_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
        }
        
        if ($this->_root->Config->DebugMode) 
            Logs::Debug('RequestType', $this->_requestType);

        $httpHeaders = $this->GetHttpHeaders();
        curl_setopt($this->_curlHandle, CURLOPT_HTTPHEADER, $httpHeaders);
        if ($this->_root->Config->DebugMode) 
            Logs::Debug('HTTP Headers', $httpHeaders);

        if (!is_null($this->_requestData)) {

            if ($this->_root->Config->DebugMode) 
                Logs::Debug('RequestData object', $this->_requestData);

            // encode to json if needed
            if (in_array(self::$_JSON_HEADER, $httpHeaders)) {
                $this->_requestData = json_encode($this->_requestData);
                if ($this->_root->Config->DebugMode) 
                    Logs::Debug('RequestData JSON', $this->_requestData);
            }

            curl_setopt($this->_curlHandle, CURLOPT_POSTFIELDS, $this->_requestData);
        }
    }
    
    /**
     * Callback to read response headers
     * @param resource $handle cURL handle
     * @param string $header Header from response
     * @return int Length of header
     */
    private function ReadResponseHeader($handle, $header) {
        
        if (strpos($header, 'X-Number-Of-Pages:') !== false) {
            $this->_pagination->TotalPages = (int)trim(str_replace('X-Number-Of-Pages:', '', $header));
        }
        
        if (strpos($header, 'X-Number-Of-Items:') !== false) {
            $this->_pagination->TotalItems = (int)trim(str_replace('X-Number-Of-Items:', '', $header));
        }
        
        if (strpos($header, 'Link: ') !== false) {

            $strLinks = trim(str_replace('Link:', '', $header));
            $arrayLinks = explode(',', $strLinks);
            if ($arrayLinks !== false) {
                $this->_pagination->Links = array();
                foreach ($arrayLinks as $link) {

                    $tmp = str_replace(array('<"', '">', ' rel="', '"'), '', $link);
                    $oneLink = explode(';', $tmp);
                    if (is_array($oneLink) && isset($oneLink[0]) && isset($oneLink[1]))
                        $this->_pagination->Links[$oneLink[1]] = $oneLink[0];
                }
            }
        }
        
        return strlen($header);
    }
    
    /**
     * Get HTTP header to use in request
     * @return array Array with HTTP headers 
     */
    private function GetHttpHeaders(){
        // return if already created...
        if (!is_null($this->_requestHttpHeaders))
            return $this->_requestHttpHeaders;
        
        // ...or initialize with default headers
        $this->_requestHttpHeaders = array();
        
        // content type
        array_push($this->_requestHttpHeaders, self::$_JSON_HEADER);
        
        // Authentication http header
        if ($this->_authRequired) {
            $authHlp = new AuthenticationHelper($this->_root);
            array_push($this->_requestHttpHeaders, $authHlp->GetHttpHeaderKey());
        }        

        return $this->_requestHttpHeaders;
    }
    
    /**
     * Check response code
     * @param object $response Response from REST API
     * @throws ResponseException If responso code not OK
     */
    private function CheckResponseCode($response){

        if ($this->_responseCode != 200) {
            
            if (isset($response) && is_object($response) && isset($response->Message)) {
                
                $error = new Error();
                $error->Message = $response->Message;
                $error->Errors = property_exists($response, 'Errors') 
                        ? $response->Errors 
                        : property_exists($response, 'errors') ? $response->errors : null;
                throw new ResponseException($this->_requestUrl, $this->_responseCode, $error);
                
            } else         
                throw new ResponseException($this->_requestUrl, $this->_responseCode);
        }
    }
}
