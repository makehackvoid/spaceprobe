\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{re2o-poster}[Re2o Poster]

\RequirePackage{kvoptions}
\SetupKeyvalOptions{prefix=re@}
\DeclareStringOption{color}[cf0f23]
\DeclareStringOption{size}[12pt]
\DeclareStringOption{logo}[logo.png]
\ProcessKeyvalOptions*
\PassOptionsToClass{\re@size}{article}
\LoadClass[a4paper]{article}
\RequirePackage[table]{xcolor}
\RequirePackage[sfdefault,lf]{carlito}
\RequirePackage[margin=1.1cm,a4paper]{geometry}
\RequirePackage{ragged2e}
\RequirePackage{graphicx}
\RequirePackage{background}


\definecolor{MainColor}{HTML}{\re@color}

\pagestyle{empty}
\RaggedRight
\parskip=\re@size plus 4pt

\renewcommand{\labelenumi}{\Huge\color{MainColor}\bfseries \arabic{enumi} }

\renewcommand{\maketitle}{{\fontsize{40pt}{42pt}\bfseries\selectfont\color{MainColor}%
\makeatletter\@title\makeatother%
\par}}

\renewcommand{\emph}[1]{{\itshape\color{MainColor}\bfseries#1}}

\newcommand{\link}[1]{{\bfseries#1}}

\backgroundsetup{
scale=1,
color=black,
opacity=0.1,
angle=0,
placement=top,
vshift=1.9cm,
contents={%
  \includegraphics[width=\paperwidth]{\re@logo}
  }%
}

%\endinput