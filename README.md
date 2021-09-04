# Trident: The multiple-pipe system

Trident provides an easy way to pipe the output of one command to not just one but many targets. These targets can be other commands, or files, and it can be filtered (line-wise) through a regular expression.

## Introduction

A trident file is split into Jobs, which consist of
 * A job name. A valid job name starts with either a letter or an underscore, followed by zero or more letters, digits or underscores. That means, job0 is a valid name, but 0job is not.
 * A command. This is any program, along with any number arguments. Arguments which contain any of the reserved words must be set in quote marks. Reserved words are JOB, CMD, OUT and FILE.
 * One or more targets. A target can either be a file, or another job. A target can also contain a regular expression, in which case the output of this job will be filtered through this RegEx, and only those lines which match are passed to the actual target.

## Syntax

To define a new job, use the keyword **JOB** followed by the job name. Then, the keyword **CMD** is followed by the program and its arguments. Finally, the keyword **OUT** indicates the start of the list of targets. A target consists of an optional RegEx (enclosed in */re/*), either the keyword **FILE** and the name of a file (absolute or relative path), or **JOB** followed by the job name. The special file names stdout and stderr are reserved for their standard usage.

To invert a regular expression (and thus output every line *not* matching it), precede it by an exclamation mark: *! /re/*.

## Examples

This example file runs the local program *./apery* (which is assumed to exist in the local directory), and passes its output to three targets. Firstly, it writes the entire output into *power_series_result.txt* unfiltered. Secondly, it also passes every line unfiltered to the job *double_check*, which runs the local program *./test* in the current directory, and writes its output to the file *test_result.txt*. Finally, it also passes all those lines that contain a negative number (indicated by the RegEx /-[0-9]/) to another job, which simply counts the number of input lines and writes the output to the screen.

```Trident
JOB compute
CMD ./apery -prec 128 -p
OUT
	- FILE power_series_result.txt
	- JOB double_check
	- /-[0-9]/ JOB count_lines

JOB double_check
CMD ./test "-"
OUT
	- FILE test_result.txt

JOB count_lines
CMD wc
OUT
	- FILE stdout
```

## Command line interface

Trident takes as command line arguments the paths to the input files, which are read in the order that they appear in. All files are considered as one continuous stream of input.

After parsing all input files, by default trident executes the first job defined in the input. This behaviour can be overwritten by using the command line option *-s* to explicitly provide a job name:
```Trident
./trident example.tr -s count_lines
```

## Compilation

Trident uses Flex and Bison/Yacc for parsing input, so both of those need to be installed.

This respoitory uses CMake as its build system, so to compile trident simply run
```bash
mkdir build
cd build
cmake ..
make -j
```
After finishing the compilation, an executable file called *trident* will be created in the directory *build*. If you want to install the program system-wide, additionally run
```
sudo make install
```
