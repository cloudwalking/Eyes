#!/usr/local/bin/php
<?php

echo constrainBetween(-2, 0, 15) == 14;  echo "\n";
echo constrainBetween(-1, 0, 15) == 15;  echo "\n";
echo constrainBetween(0, 0, 15) == 0;  echo "\n";
echo constrainBetween(1, 0, 15) == 1;  echo "\n";
echo constrainBetween(2, 0, 15) == 2;  echo "\n";
echo constrainBetween(3, 0, 15) == 3;  echo "\n";
echo constrainBetween(14, 0, 15) == 14;  echo "\n";
echo constrainBetween(15, 0, 15) == 15;  echo "\n";
echo constrainBetween(16, 0, 15) == 0;  echo "\n";
echo constrainBetween(17, 0, 15) == 1;  echo "\n";

echo constrainBetween(14, 16, 31) == 30;  echo "\n";
echo constrainBetween(15, 16, 31) == 31;  echo "\n";
echo constrainBetween(16, 16, 31) == 16;  echo "\n";
echo constrainBetween(17, 16, 31) == 17;  echo "\n";
echo constrainBetween(18, 16, 31) == 18;  echo "\n";
echo constrainBetween(30, 16, 31) == 30;  echo "\n";
echo constrainBetween(31, 16, 31) == 31;  echo "\n";
echo constrainBetween(32, 16, 31) == 16;  echo "\n";
echo constrainBetween(33, 16, 31) == 17;  echo "\n";

function constrainBetween($value, $lower, $higher) {
  if ($value < $lower) {
    $value = $higher - ($lower - $value) + 1;
  } else if ($value > $higher) {
    $value = $lower + ($value - $higher) - 1;
  }
  return $value;
}

?>