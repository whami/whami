<?php
/**
 * @file
 * Default theme implementation to display the licence information of a external place.
 *
 * Available variables:
 *
 * - $whami_place: The place object itself. You will only need to use it when the
 *   following variables don't suffice.
 * - $whami_op: What are we theming here? teaser, body, etc.
 * - Therefore the main content is in $whami_place[$whami_op]
 *
 */

?>
<div class="whami-place-teaser">

  <table>
    <tr><td>
      <span><b><?php print $whami_place['title']; ?></b></span>
    </td></tr>

    <?php if($whami_place_type) : ?>
    <tr><td>
      <div><?php print $whami_place_type; ?></div>
    </td></tr>
    <?php endif;?>

    <tr><td>
      <span><?php print $whami_place['body']; ?></span>
    </td></tr>

    <tr><td>
        <?php if($whami_place_licence) : ?>
          <?php print $whami_place_licence; ?>
        <?php endif;?>
    </td></tr>
  </table>

</div>