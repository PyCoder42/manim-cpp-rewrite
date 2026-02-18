.. Documentation template with example section.
.. See: https://github.com/manim-cpp/manim-cpp/wiki/documentation-guidelines

SomeClass
=========

A one line description of the class.

A short paragraph providing more details.

Extended Summary
----------------

Parameters
----------
scale_factor
    The factor used for scaling.

Returns
-------
:class:`~.VMobject`
    Returns the modified :class:`~.VMobject`.

Raises
------
:class:`TypeError`
    If one element of the list is not an instance of VMobject.

Examples
--------
.. manim:: AddTextLetterByLetterScene
    :save_last_frame:

    class AddTextLetterByLetterScene(Scene):
        def construct(self):
            t = Text("Hello World word by word")
            self.play(AddTextWordByWord(t))

See Also
--------
:class:`Create`, :class:`~.ShowPassingFlash`
