var whami_places = whami_places || new Array();

//function whami_collect_places(map) {
//	
//	//Get place for a node map
//	for (var url_hash in Drupal.settings.whami.map_places[map.mapid]) {
//		var whami_location = Drupal.settings.whami.map_places[map.mapid][url_hash];
//		whami_places[url_hash] = {'place_data': whami_location};
//	}
//	Drupal.settings.mapstraction_formatter.maps[map.mapid].features = {};
//
//}

function whami_collect_places(map) {
	
	//Get place for a node map
	for (var key in Drupal.settings.whami.map_places[map.mapid]) {
		var url_hash = Drupal.settings.whami.map_places[map.mapid][key];
		
//		if(typeof Drupal.settings.whami.places[url_hash] == 'object')
//			var whami_location = Drupal.settings.whami.places[url_hash];
//		else
//			var whami_location = Drupal.settings.whami.node_map_place[map.mapid];
		
//		whami_places[url_hash] = {'place_data': whami_location};
		whami_places.push(url_hash);
	}
	Drupal.settings.mapstraction_formatter.maps[map.mapid].features = {};
	whami_create_markers(whami_places, map);

}

var whami_markers = whami_markers || new Array();
function whami_create_markers(places, map) {
	
	//loop over members of whami_location_data object
	for (var url_hash in places) {
		
		//Check if we have the place object or the url_hash
		
		//1. if we have the url_hash -> get the place
		if(typeof places[url_hash] == 'string') {
			url_hash = places[url_hash];
			places[url_hash] = Drupal.settings.whami.places[url_hash];
		}

		if(typeof places[url_hash] == 'object') {
			if(typeof Drupal.settings.whami.places[url_hash] != 'object') 
				Drupal.settings.whami.places[url_hash] = places[url_hash];
			if(typeof Drupal.settings.whami.places[url_hash].markerInstance == 'object') continue;
		} else
			places[url_hash] = {'place_data': Drupal.settings.whami.node_map_place[map.mapid]};
		//make sure that we have a valid whami object
//		if(typeof places[url_hash] != 'object'){continue;}		
		if(typeof places[url_hash].place_data != 'object'){continue;}
		
		data = places[url_hash].place_data;
		//make sure that we don't have a marker at the same place
		if(whami_markers[data.latitude + ',' + data.longitude]){continue;}			

		//make sure that we instantiate the place
		if(typeof Drupal.settings.whami.places[url_hash] != 'object')
			Drupal.settings.whami.places[url_hash] = {};
		//make sure that we save the place data
		if(typeof Drupal.settings.whami.places[url_hash].place_data != 'object')
			Drupal.settings.whami.places[url_hash].place_data = data;
		
		var point = new mxn.LatLonPoint(data.latitude, data.longitude);
		var marker = new mxn.Marker(point);
		marker.setLabel(data.title);
		
		// check if custom icon path has been specified and use it if it is specified
		if ( data.hasOwnProperty('map_display_icon') )
			marker.setIcon(data.map_display_icon);
	
		// add the node's title to label tag which shows up when the mouse cursor is over the marker 
		if ( data.hasOwnProperty('infoBubble') && data.infoBubble.length )
			marker.setInfoBubble(data.infoBubble);
		else if ( data.hasOwnProperty('full_url') && data.hasOwnProperty('title') && data.hasOwnProperty('body') )
			marker.setInfoBubble('<h2><a href="' + data.full_url + '">' + data.title + '</a></h2>' + data.body);						
		// add the node's title to label tag which shows up when the mouse cursor is over the marker 
		else if ( data.hasOwnProperty('title') && data.hasOwnProperty('body') )
			marker.setInfoBubble('<h2>' + data.title + '</h2>' + data.body);
		// add the "info bubble" that shows up when the marker has been clicked on
		else if ( data.hasOwnProperty('body') )
			marker.setInfoBubble(data.body);
			
		Drupal.settings.whami.places[url_hash].mapid = map.mapid;
		Drupal.settings.whami.places[url_hash].markerInstance = marker;
		Drupal.settings.whami.places[url_hash].markerType = 'mxn';
		
		whami_markers[data.latitude + ',' + data.longitude] = {'markerInstance':marker,'markerType':'mxn'};
		map.addMarker(marker);
	}
}

var whami_maps = whami_maps || new Array();

var Mapstraction = Mapstraction || {};
Mapstraction.hooks = Mapstraction.hooks || {};
Mapstraction.hooks.whami_map_toolkit_init = function (map_definition) {
	
//	debugger;
	
	var map = map_definition.map_object.map;
	
	map.enableScrollWheelZoom();
	
	// Kartenart hybrid setzen
	if(map.maps.google) {
		var GMap = map.maps.google;
		GMap.setMapType(G_HYBRID_MAP);		
	}

//	console.log(map.markers);	
	whami_collect_places(map);
	
//	whami_create_markers(whami_places, map);
	
	if(Drupal.settings.whami.node_map_place[map.mapid]) {

		node_marker = Drupal.settings.whami.places[Drupal.settings.whami.node_map_place[map.mapid].url_hash].markerInstance;		
		
		whami_maps[map.mapid] = {};
//		whami_maps[map.mapid].lat = parseFloat(node_marker.location.lat);
//		whami_maps[map.mapid].lon = parseFloat(node_marker.location.lon);
//		whami_maps[map.mapid].center_point = new mxn.LatLonPoint(whami_maps[map.mapid].lat, whami_maps[map.mapid].lon);
//		whami_maps[map.mapid].radius = new mxn.Radius(whami_maps[map.mapid].center_point, 10);
//		
		radius = Drupal.settings.whami.map_radius[map.mapid];
//		whami_maps[map.mapid].polyline = whami_maps[map.mapid].radius.getPolyline(radius * 0.25, '#990066');
		whami_maps[map.mapid].polyline = whami_set_georadius(node_marker.location.lat, node_marker.location.lon, radius * 0.25, map);
//		map.addPolyline(whami_maps[map.mapid].polyline);
		
		node_bubble_handler = function(event,marker){
//			debugger;
//			console.log("Bubble: " + marker.mapstraction.mapid);
			
			//This code should run one time for the initialization of the map
			if(whami_maps[marker.mapstraction.mapid]['init']) return;
			marker.mapstraction.removePolyline(whami_maps[marker.mapstraction.mapid].polyline);
			whami_maps[marker.mapstraction.mapid]['init'] = true;
			
//			marker.openBubble();
		};
		node_marker.openInfoBubble.addHandler(node_bubble_handler, map);
		
		node_marker.openBubble();
	}
	else {
		whami_maps[map.mapid]['init'] = true;
	}
	
//	  if (Drupal.settings.whami.whami_map_cluster == map.mapid) {
//		  whami_map_cluster(map_definition);
//	  }
	
};

//helperfunction to set marker bubble
function whami_set_marker_bubble(infoBubbleHtml, marker, markerType, map) {
	
	infoBubbleHtml instanceof jQuery ? infoBubbleHtml = infoBubbleHtml.wrap('<div>').parent().html() : infoBubbleHtml = infoBubbleHtml;
	
	if(markerType == 'mxn') {
    	map.removeMarker(marker, true);
    	
    	//Get changed bubble-HTML from DOM
    	marker.setInfoBubble(infoBubbleHtml);
    	
    	map.addMarker(marker);
    	marker.openBubble();
	}
	
	if(markerType == 'gmap') {
		marker.closeInfoWindow();
		marker.bindInfoWindowHtml(infoBubbleHtml);
		marker.openInfoWindowHtml(infoBubbleHtml);
	}
}

function whami_set_message_to_bubble_txt(infoBubble, message, type) {
	if ( type == undefined ) {
		type = 'normal';
	}
//	var url_hash = $(".whami-place .place-link", dom_infoBubble).attr("title");
	
	infoBubble instanceof jQuery ? dom_infoBubble = infoBubble : dom_infoBubble = $(infoBubble);
	
	message = '<div class="messages ' + type + '">' + message + '</div>';
	$(".whami-action-links", dom_infoBubble).html(message);
	
	return dom_infoBubble.wrap('<div>').parent();
}

function whami_set_georadius(lat, lon, radius, map) {
	lat = parseFloat(lat);
	lon = parseFloat(lon);
	center_point = new mxn.LatLonPoint(lat, lon);
	mxnRadius = new mxn.Radius(center_point, 10);
	
	polyline = mxnRadius.getPolyline(radius, '#990066');
	
	map.addPolyline(polyline);
	
	return polyline;
}

//function whami_add_to_dom_temp(content) {
//	$('body').append("<div id=\"whami-temp-bubble\">");
//	var dom_infoBubble = $("#whami-temp-bubble");
//	dom_infoBubble.append(content);
//	return dom_infoBubble;
//}