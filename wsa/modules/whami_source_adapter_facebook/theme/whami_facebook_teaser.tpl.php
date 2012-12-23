<?php


?>

<?php if($whami_icon) : ?>
  <div class='fb_<?php print $object_type; ?>_icon'>
    <?php print $whami_icon; ?>
  </div>
<?php endif;?>

<?php if($whami_title) : ?>
  <div class='fb_<?php print $object_type; ?>_title'>
    <?php print $whami_title; ?>
  </div>
<?php endif;?>

<?php if($whami_message) : ?>
  <div class='fb_<?php print $object_type; ?>_message'>
    <?php print $whami_message; ?>
  </div>
<?php endif;?>
