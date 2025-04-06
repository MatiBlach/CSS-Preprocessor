# CSS-Preprocessor
Simple CSS processing engine.

### CSS
+ The processing starts with reading the CSS declarations. The CSS is syntactically correct and consists of an attribute block, optionally preceded by selectors. The absence of selectors is valid (it would mean the attributes apply to everything).

+ Selectors are separated by commas. Only selectors valid in CSS are allowed, but we can assume that they do not contain commas or curly braces.

+ The attribute block is enclosed in curly braces.

+ Attributes are separated by semicolons and consist of a property name and a value, separated by a colon. The last attribute in a block may be followed by a semicolon, but it is not required.

+ Attribute values can be any valid CSS construct, but to simplify things, we can safely assume that any strings are not malicious — i.e., they do not contain escaped quotation marks, curly braces, or semicolons.

+ If a given attribute (property name) appears more than once in a block, it should be treated as a single occurrence, where only the last value is significant.

+ Selectors, attribute names, and attribute values do not require semantic interpretation — we treat them as plain values (after trimming leading/trailing whitespace). For example: 'margin-left: 8px', 'margin: 4px 7px 4px 7px'
These are considered separate, unrelated attributes with names margin-left and margin, and values '8px' and '4px 7px 4px 7px', respectively.

+ Similarly, selectors are treated as values and do not require interpretation. For example, h1 and h1.theme are considered distinct, unrelated selectors.

#### Simplifications:

+ CSS does not contain comments or @-type selectors.

+ Blocks cannot be nested.

+ For testing purposes, you may assume that no selector or attribute is split across multiple lines (although a single line may contain multiple separators and/or attributes).

### Commands
In the commands below, i and j are positive integers (fitting in an int), and n is a valid attribute name.

```????``` – start of the command section

```****``` – resume reading CSS

```?``` – print the number of CSS blocks

```i,S,?``` – print the number of selectors in block i (blocks are 1-indexed); if the block doesn't exist, skip

```i,A,?``` – print the number of attributes in block i; if the block doesn't exist, skip

```i,S,j``` – print the j-th selector of block i (both blocks and attributes are 1-indexed); if not present, skip

```i,A,n``` – print the value of attribute named n in block i; if not found, skip

```n,A,?``` – print the total number of occurrences of the attribute named n (duplicates in the same block are removed during loading). Zero is allowed

```z,S,?``` – print the total number of occurrences of selector z across all blocks. Zero is allowed

```z,E,n``` – print the value of the attribute named n for selector z; if multiple blocks use selector z, use the last one; if not found, skip

```i,D,*``` – delete the entire block i (i.e., selectors and attributes); on success, print deleted

```i,D,n``` – delete the attribute named n from block i; if this results in an empty block, delete the whole block including selectors; on success, print deleted

# Example
### Input
```
#breadcrumb 
{
	width: 80%;
	font-size: 9pt;
}

h1, body {
	min-width: 780px;
	margin: 0;
	padding: 0;
	font-family: "Trebuchet MS", "Lucida Grande", Arial;
	font-size: 85%;
	color: #666666;
}

h1, h2, h3, h4, h5, h6 {color: #0066CB;}
????
?
1,S,?
1,S,1
1,A,?
2,A,font-family
h1,S,?
color,A,?
h1,E,color
1,A,padding
1,D,*
?
2,D,color
?

****
h1, h2, h3, h4, h5, h6 {color: #0066FF;}
????
?
```
### Output
```
? == 3
1,S,? == 1
1,S,1 == #breadcrumb
1,A,? == 2
2,A,font-family == "Trebuchet MS", "Lucida Grande", Arial
h1,S,? == 2
color,A,? == 2
h1,E,color == #0066CB
1,D,* == deleted
? == 2
2,D, color == deleted
? == 1
? == 2
```
