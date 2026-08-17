"""TeX escaping used when emitting listings for pdflatex."""


def escape(text: str) -> str:
    text = text.replace("<", r"\ensuremath{<}")
    text = text.replace(">", r"\ensuremath{>}")
    return text


def pathescape(text: str) -> str:
    text = text.replace("\\", r"\\")
    text = text.replace("_", r"\_")
    return escape(text)


def codeescape(text: str) -> str:
    text = text.replace("_", r"\_")
    text = text.replace("\n", "\\\\\n")
    text = text.replace("{", r"\{")
    text = text.replace("}", r"\}")
    text = text.replace("^", r"\ensuremath{\hat{\;}}")
    return escape(text)


def ordoescape(text: str, esc: bool = True) -> str:
    if esc:
        text = escape(text)
    start = text.find("O(")
    if start >= 0:
        bracketcount = 1
        end = start + 1
        while end + 1 < len(text) and bracketcount > 0:
            end = end + 1
            if text[end] == "(":
                bracketcount += 1
            elif text[end] == ")":
                bracketcount -= 1
        if bracketcount == 0:
            return r"%s\bigo{%s}%s" % (
                text[:start],
                text[start + 2 : end],
                ordoescape(text[end + 1 :], False),
            )
    return text
