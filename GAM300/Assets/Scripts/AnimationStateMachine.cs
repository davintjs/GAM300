using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

namespace BeanFactory
{
    public class AnimationState
    {
        public bool canTransition
        {
            get
            {
                foreach (AnimationState animState in trueConditionals)
                {
                    if (animState.state == false)
                    {
                        return false;
                    }
                }

                foreach (AnimationState animState in falseConditionals)
                {
                    if (animState.state == true)
                    {
                        return false;
                    }
                }

                return state;
            }
        }

        public void SetConditionals(bool condition, params AnimationState[] animationStates)
        {
            foreach (var state in animationStates)
            {
                if (condition)
                {
                    trueConditionals.Add(state);
                }
                else
                {
                    falseConditionals.Add(state);
                }
            }
        }

        public bool stall = false;
        public bool state = false;
        public bool loop = false;
        public float speed = 1f;
        List<AnimationState> trueConditionals = new List<AnimationState>();
        List<AnimationState> falseConditionals = new List<AnimationState>();
    }

    public class AnimationStateMachine
    {
        Animator animator;
        Dictionary<string, AnimationState> animationStates = new Dictionary<string, AnimationState>();
        string currentState = "";

        public AnimationStateMachine (Animator _animator)
        {
            animator = _animator;
            animator.SetDefaultState("Idle");
            animator.Play();
        }

        //Call this in update function to update the animation state
        public void UpdateState()
        {
            AnimationState currState = GetState(currentState);
            if (currState != null && currState.stall)
            {
                if (animator.GetProgress() <= 0.95f)
                {
                    return;
                }
                else
                {
                    currState.state = false;
                }
                currState = null;
                currentState = "";
            }
            foreach (var pair in animationStates)
            {
                if (pair.Value.canTransition)
                {
                    if (currentState == pair.Key)
                    {
                        if (currState != null && currState.loop)
                        {
                            animator.SetNextState(currentState);
                        }
                        return;
                    }
                    currentState = pair.Key;
                    animator.SetState(currentState);
                    animator.SetSpeed(pair.Value.speed);    
                    return;
                }
            }
            if (currentState != "")
            {
                currentState = "";
                animator.SetState("Idle");
                animator.SetSpeed(1f);
            }
        }

        public AnimationState GetState(string name)
        {
            if (name == "")
                return null;
            if (!animationStates.ContainsKey(name))
                animationStates.Add(name, new AnimationState());
            return animationStates[name];
        }

        public AnimationState GetCurrentState()
        {
            return animationStates[currentState];
        }

    }
}

