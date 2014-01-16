///////////////////////////////////////////////////////////////////////////
// Animation
// - General purpose interface for property-based animation
// - Useful for animation of 2D elements (decals, interface elements, etc.),
//   but possibly has other uses as well
////////////////////////////////////////////////////////////////////////////

namespace Arya
{

    // Classes with animatable properties should inherit this class
    class Animatable
    {
        public: 
            Animatable() { }
            virtual ~Animatable() { }

            virtual void setValue();
    };


    // A 'controller' that controls the animation and has an reference
    // to the animatable object. The T class should have support for 
    // basic arithmetic functions
    template<class T> 
    class PropertyAnimation
    {
        public:
            PropertyAnimation() { }
            ~PropertyAnimation() { }

            T getCurrentValue();
            T getFinalValue();

        private:
            float t;
            float t_end;

            T initialValue;
            T finalValue;

            Animatable* obj;
    };

    // some kind of controller that has a list of active animations
    // and sends an update every frame??

    class AnimationController : public FrameListener, public Singleton<Interface>
    {
        public:
            AnimationController();
            virtual ~AnimationController();
    };
}
