use strict;
use warnings;
use File::Copy;

my $dir = ".";
my $index = 0;
my $newfilename = "";
opendir DIR, $dir or die "cannot open dir $dir: $!";
my @file= readdir DIR;
foreach my $file (@file) {
	if ($file =~ /[0-9]{4}/) {
		$newfilename = "smoke" . sprintf("%04i", $index) . ".png";
		print "move $dir/$file to $dir/$newfilename\n";
		move("$dir/$file", "$dir/$newfilename") or die(qq{failed to move $dir/$file -> $dir/$newfilename});
		$index++;
	}
}
closedir DIR;