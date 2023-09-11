<head>
  <link rel="stylesheet" href="./styles.css">
</head>
<body>
<p>
## stoney-css

Some cool CSS layered framework stuff based on the invaluable tutelage of MDN Web docs - more documentation on this repo to come soon.

This framework was designed by querying the unneccessary complexities that tend to over-populate the cascaded stylesheet convention, and attempts to minimize and clarify the information that makes it's way to your HTML elements by exposing all of the CSS property defaults that a client's browser's user agent would provide, similar to the popular "CSS Reset" stylesheets, but while also taking advantage of more modern CSS3 capabilities such as @layers, @support (for webkit, opera, moz, et al), custom animation/transition @keyframes templates, and more; and thanks to these modern CSS capabilities it does a pretty good job of keeping Chromium Devtools console feedback clear, concise and logical. With good practice, the result is "far less CSS" and a more focused appraoch to styling your markups.

Crucially, it's all done using native CSS3 - no plugins, no libraries, no additional dependencies nor build step...

Just put these files where your CSS is (i.e., './src/assets/css/stoney-css'), @import the 'styles.css' within, and begin styling your page exactly as you normally would. Like this:

```
@import url("./stoney-css/styles.css");


/* Begin here... */

.container {
  display: flex;
}

img.logo {
  height: 50px;
  width: 300px;
}

/* Etc... */
```

It's a good idea to break your page down into 'components', such as header, footer, landing article, image gallery, contact section... etc. You can make some custom site-wide defaults in the 'components' folder, use the '@import url() layer();' syntax as seen below (from stoney-css/styles.css), and then over-ride those on a per-usage basis in your own master css files. This concept somewhat resembles inheritance in OOP...

```
@charset "UTF-8";
@import url("../../../../public/fonts/microsoft/cascadia/font-cascadia-dynamic.css") layer(fontFace);
/* @import url("./framework/reset.css") layer(reset); */
@import url("./framework/support.css") layer(defaults.support);
@import url("./framework/defaults.css") layer(defaults);
@import url("./framework/global.css") layer(global);
/* @import url("./framework/helpers.css") layer(global.helpers); */
@import url("./framework/animations/animations.css") layer(site.animations);
@import url("./framework/site.css") layer(site);
@import url("./framework/components/header.css") layer(header);
@import url("./framework/components/footer.css") layer(footer);

/* @import url("./framework/debug/dbg.css") layer(site.dbg); */
/* @import url("./framework/debug/lint.css") layer(site.dbg.lint); */
```

When you check your computed CSS styles in Chromium Devtools, the @layer syntax will show you exactly where the defaults were specified (and what they were), and exactly where they have been over-ridden. In most cases, the total query list shall usually be a maximum of two sources.

Without such a strategy, it is notoriously easy to lose track of specificity and the ability to control markup styles quickly gets out of hand, meaning revisions and refactors are more likely than anybody would wish for (IMO this is what gives rise to the over-dependence on third-party libs to perform native CSS coding tasks).

Responsive text in CSS:
```
body {
  font-size: calc(15px + 0.390625vw);
}
h1,
h2,
h3,
h4,
h5,
h6,  {
  display: block;
  margin-left: 0;
  margin-right: 0;
  font-weight: bold;
}
h1 {
  font-size: 2.0em;
  margin-top: 0.67em;
  margin-bottom: 0.67em;
}
h2 {
  font-size: 1.5em;
  margin-top: 0.83em;
  margin-bottom: 0.83em;
}
h3 {
  font-size: 1.17em;
  margin-top: 1.0em;
  margin-bottom: 1.0em;
}
h4 {
  margin-top: 1.33em;
  margin-bottom: 1.33em;
}
h5 {
  font-size: 0.83em;
  margin-top: 1.67em;
  margin-bottom: 1.67em;
}
h6 {
  font-size: 0.67em;
  margin-top: 2.33em;
  margin-bottom: 2.33em;
}
```

references: <a href="https://developer.mozilla.org/en-US/docs/Web/CSS/@layer">Mdn Web Docs: CSS @layers</a>
</p>
</body>
