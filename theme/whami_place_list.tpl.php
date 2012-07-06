<?php
/**
 * @file
 * Default theme implementation to display a list of places.
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
<div class="whami-place-list">

        <?php if($whami_places) : ?>
          <?php print $whami_places; ?>
        <?php endif;?>

</div>