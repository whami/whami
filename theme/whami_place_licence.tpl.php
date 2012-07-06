<?php
// $Id: flag.tpl.php,v 1.1.2.7.2.2 2009/10/28 00:01:32 quicksketch Exp $

/**
 * @file
 * Default theme implementation to display the licence information of a external place.
 *
 * Available variables:
 *
 * - $place: The place object itself. You will only need to use it when the
 *   following variables don't suffice.
 * - $whami_long_url: The full url to the place on external website
 * - $whami_short_url: The shrinked URL to display for the user
 * - $whami_ext_place_link: Themed "Read more on..." - link
 * - $place['licence']
 *
 * NOTE: This template spaces out the <span> tags for clarity only. When doing some
 * advanced theming you may have to remove all the whitespace.
 */

?>
<div class="whami-place-licence" style='margin-top:0'>

  <div class="whami-read-more" style='margin-top:0'>

  <table> <!-- style='margin-bottom:3px' -->

  <?php if($whami_place_icon) : ?>
  <colgroup>
    <col width="35">
    <!--    <col width="100">-->
    <!--    <col width="320">-->
  </colgroup>
  <?php endif;?>

  <tr>

        <?php if($whami_place_icon) : ?>
         <td>
          <?php print $whami_place_icon; ?>
         </td>
        <?php endif;?>

      <td align="left" valign="middle">
      <?php print $whami_ext_place_link; ?>
      <?php if (!$whami_place['own_whami_content']) :?>
        <div title='<?php print $whami_long_url; ?>' style='margin-top:0'><?php print $whami_short_url; ?></div>
      <?php endif;?>
      </td>

  </tr>
  </table>

  </div>

  <?php if(strlen($whami_place['licence'])) : ?>
    <div class="whami_read_licence">
    <?php print $whami_place['licence']; ?>
    </div>
  <?php endif;?>

</div>