<?php

//module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook.admin');

function whami_source_facebook_get_fb_token($type = 'user') {
  global $_fb;

  if (isset($_fb)) {
    if (!strcmp($type, 'user')) {
      $facebook_user = fb_facebook_user();
    }

    $fb_access_token = fb_get_token(NULL, $facebook_user);
  }
  elseif ($GLOBALS['whami_fb_access_token']) {
    $fb_access_token = $GLOBALS['whami_fb_access_token'];
  }
  elseif (module_exists('whami_facebook_client')) {
    $fb_access_token = whami_facebook_client_token();
  }
  else {
    module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook.admin');
    $fb_access_token = _whami_source_adapter_facebook_tmp_token();
  }

  return strlen($fb_access_token) ? $fb_access_token : FALSE;
}

function whami_facebook_get_fb_token_infos($fb_access_token = NULL, $type = NULL) {

  static $cached_results = array();

  if(!is_array($type)) {
    if(strlen($type))
      $type = array($type);
    else
      $type = array('user', 'app');
  }

  if(is_numeric(array_search('user', $type))) {
    if(!$fb_access_token) $fb_access_token = whami_source_facebook_get_fb_token('user');
    if($cached_results[$fb_access_token . 'user']) return $cached_results[$fb_access_token . 'user'];

    //http://developers.facebook.com/docs/authentication/
    //programmatically check which permissions a user has granted your application:
    //https://graph.facebook.com/me/permissions?access_token=USER_ACCESS_TOKEN

    $param['access_token'] = $fb_access_token;
    $fb_query_data = whami_execute_graph_api_request('me/permissions', $param, $fb_access_token);

    if ($data = print_r($fb_query_data, true)) {
      $cached_results[$fb_access_token . 'user'] = $data;
      return $data;
    }
  }
  if(is_numeric(array_search('app', $type))) {
    if(!$fb_access_token) $fb_access_token = whami_source_facebook_get_fb_token('app');
    if($cached_results[$fb_access_token . 'app']) return $cached_results[$fb_access_token . 'app'];

    //https://developers.facebook.com/docs/authentication/applications/
    //To retrieve the details of your application
    //https://graph.facebook.com/app?access_token=APP_ACCESS_TOKEN

    $param['access_token'] = $fb_access_token;
    $param['fields'] = 'id,name,app_domains,canvas_url';
    $param['metadata'] = 1;
    $fb_query_data = whami_execute_graph_api_request('app', $param, $fb_access_token);

    if($fb_query_data['id'] && !strcmp($fb_query_data['metadata']['type'], 'application')
        && isset($fb_query_data['app_domains']) && isset($fb_query_data['canvas_url'])) {

//      $fb_query_data = whami_execute_graph_api_request($fb_query_data['id'], $param, $fb_access_token);

      unset($fb_query_data['metadata']);
      if ($data = print_r($fb_query_data, true)) {
        $cached_results[$fb_access_token . 'app'] = $data;
        return $data;
      }
    }
  }

    return t('Token type is not valid');
}

function whami_execute_graph_api_request($method, $param, $fb_access_token = NULL) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook.admin');

  if ($fb_access_token || $fb_access_token = whami_source_facebook_get_fb_token('app')) {
    $param['access_token'] = $fb_access_token;
  }

  //compose the HTTP request and execute it
  $fb_query_url = 'https://graph.facebook.com/' . $method . '?' . http_build_query($param, '', '&');
  $fb_query_result = drupal_http_request($fb_query_url);

  if ($fb_query_result->code == 200 && isset($fb_query_result->data)) {
    $fb_query_data = json_decode($fb_query_result->data, TRUE);
    if (_whami_source_adapter_facebook_debug()) {
      whami_write_debug_log($fb_query_url, $fb_query_data, $param);
    }
    return $fb_query_data;
  }
  else {
    if (_whami_source_adapter_facebook_debug()) {
      whami_write_debug_log($fb_query_url, $fb_query_result, $param);
    }
  }
}

function whami_execute_fql_request($fb_fql_multiquery, $fb_access_token = NULL) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook.admin');

  if ($fb_access_token || $fb_access_token = whami_source_facebook_get_fb_token('app')) {
    $fb_fql_multiquery_params['access_token'] = $fb_access_token;
  }

  $fb_fql_multiquery_params['q'] = json_encode((object) $fb_fql_multiquery);
  $fb_fql_multiquery_params['format'] = 'json-strings';

  // compose the HTTP request and execute it
  $fb_fql_multiquery_url = 'https://graph.facebook.com/fql?' . http_build_query($fb_fql_multiquery_params, '', '&');
  $fb_fql_multiquery_result = drupal_http_request($fb_fql_multiquery_url);

  if ($fb_fql_multiquery_result->code == 200 && isset($fb_fql_multiquery_result->data)) {
    $fb_fql_multiquery_data = json_decode($fb_fql_multiquery_result->data, TRUE);
    if (_whami_source_adapter_facebook_debug()) {
      whami_write_debug_log($fb_fql_multiquery_url, $fb_fql_multiquery_data, $fb_fql_multiquery_params);
    }
    if (!empty($fb_fql_multiquery_data['data'])) return $fb_fql_multiquery_data['data'];
  }
  else {//HTTP request to Facebook API backend failed
    if (_whami_source_adapter_facebook_debug()) {
      whami_write_debug_log($fb_fql_multiquery_url, $fb_fql_multiquery_result, $fb_fql_multiquery_params);
    }
  }
}

function whami_write_debug_log($name, $value, $param = NULL) {
    if($GLOBALS['user']->whami_facebook_log) $log_data = $GLOBALS['user']->whami_facebook_log;

    $log_data[$name] = $param ? array('param' => $param) : array();
    $log_data[$name]['result'] = $value;

    $GLOBALS['user']->whami_facebook_log = $log_data;
    user_save($GLOBALS['user'], array('whami_facebook_log' => $log_data));
}

function whami_print_debug_log() {
  if($GLOBALS['user']->whami_facebook_log) {
    if(function_exists('dpm') && _whami_source_adapter_facebook_debug())
      dpm($GLOBALS['user']->whami_facebook_log);

    unset($GLOBALS['user']->whami_facebook_log);
    user_save($GLOBALS['user'], array('whami_facebook_log' => NULL));;
  }
}

function whami_facebook_get_place_url($fb_id) {
  return whami_facebook_get_url_prefix() . $fb_id;
}

function whami_facebook_get_event_url($fb_id) {
  return whami_facebook_get_url_prefix() . 'events/' . $fb_id;
}

function whami_facebook_get_feed_url($fb_id) {
  //http://www.facebook.com/feeds/page.php?id=50112151664&format=rss20
  //50112151664 ist dabei die variable Place-Nummer.
  return whami_facebook_get_url_prefix() . 'feeds/page.php?id=' . $fb_id . '&format=rss20';
}

function whami_facebook_get_id_from_url($url) {
  //http://www.facebook.com/140037009377135
  return drupal_substr($url, strlen(whami_facebook_get_url_prefix()));
}

function whami_facebook_get_url_prefix() {
  return "http://www.facebook.com/";
}
