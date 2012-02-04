whami source-adapter

Use case
In the context of nodes with geo-information – for example using maps -
there is pretty often the requirement to display further
geo-related information in the same area as the
geo-information of the node itself.
´whami source-adapter´delivers a generic method fulfilling this requirement
together with two source adapters, one for Panoramio, one for Wikipedia.
This module was part of the development of whami.com and can easily
be extented with further source adapters.

Description
‘whami source-adapter’ delivers two configurable
display blocks for Wikipedia/Panoramio-content which is locally
close to the geo-information of the node.

Panoramio source adapter
The Panoramio source adapter selects photos via the Panoramio REST API
using the following parameters:
- Photo type public (not configurable)
- number of displayed photos (configurable)
- for the preview the size ´small´ is used (not configurable)
- for the lightbox-display the size ´medium´ is used (not configurable)
The selected photos are displayed in small size in a block.
By clicking on the photo you get a medium size display of the photo
in a lightbox. The narrative of the medium size picture shows a link
to the photo and author page of the photo.

Wikipedia source adapter
The Wikipedia source adapter selects articles near the zoomed area
from the Geonames service on www.geonames.com.
This is done via the Geonames module.
We recommend to use the premium service of Geonames since it is more reliable.

Configuration
The module has configuration screens within the block configuration
for each block:

    the definition of the geofield which is used within the node
    the radius used for the proximity search around the node´s geofield
    the number of small photos in the block (max. 100)

Dependencies

    Lightbox 2.0
    Geonames
    Geo
    CCK

The module has successfully been tested on Drupal 6.22
with the Geonames 1.5 version, the Geo 6.x-1.0-alpha5 version,
the CCK 6.x-3.x-dev version and
the Lightbox2 6.x-1.x-dev version.

References
You can use the website of whami.com as a reference for the working module.
If you should require additional source adapters
please contact the maintainers of the module.

NOTE:
Neither the maintainers of this module nor whami.com are liable
for your usage of this module. You are clearly advised to follow
all the terms of service of Panoramio and Wikipedia:
http://www.panoramio.com/api/data/terms.html
http://www.panoramio.com/api/data/api.html
http://wikimediafoundation.org/wiki/Terms_of_Use

This module is sponsored by whami.com. Maintainers are vlad.k and martin.l.
