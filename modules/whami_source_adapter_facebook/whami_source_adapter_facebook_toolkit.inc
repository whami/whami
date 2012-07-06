<?php

module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook.admin');

function whami_source_facebook_get_fb_token() {
    global $_fb;

    if (isset($_fb)) {
      $fb_access_token = fb_get_token(NULL, fb_facebook_user());
    }
    elseif ($GLOBALS['whami_fb_access_token']) {
      $fb_access_token = $GLOBALS['whami_fb_access_token'];
    }
    else {
      $fb_access_token = _whami_source_adapter_facebook_tmp_token();
    }

    if (strlen($fb_access_token))
      return $fb_access_token;
    else
      return false;
}

function whami_execute_graph_api_request($method, $param) {

  if ($fb_access_token = whami_source_facebook_get_fb_token()) {
      $param['access_token'] = $fb_access_token;
  }

  //compose the HTTP request and execute it
  $fb_query_url = 'https://graph.facebook.com/' . $method . '?' . http_build_query($param, '', '&');
  $fb_query_result = drupal_http_request($fb_query_url);

  if ($fb_query_result->code == 200 && isset($fb_query_result->data)) {
    $fb_query_data = json_decode($fb_query_result->data, TRUE);

    if (_whami_source_adapter_facebook_debug()) {
      drupal_set_message(
        'whami fb count: ' . count($fb_query_data['data']) .
          ' Data: ' . print_r($fb_query_data['data'], true) .
          ' Token: ' . $fb_access_token
      );
    }

    return $fb_query_data;
  }
  else {
   $fb_query_data = json_decode($fb_query_result->data, TRUE);
   drupal_set_message(
    'whami fb Error: ' . $fb_query_result->code .
      ' Message: ' . $fb_query_data['error']['message'] .
      ' Type: ' . $fb_query_data['error']['type'],
    'error'
   );
  }

}

function whami_source_facebook_get_id_from_url($url) {
  //http://www.facebook.com/140037009377135
  return drupal_substr($url, strlen(whami_source_facebook_get_url_prefix()));
}

function whami_source_facebook_get_url_prefix() {
  return "http://www.facebook.com/";
}