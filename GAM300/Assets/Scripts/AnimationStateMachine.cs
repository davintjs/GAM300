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
        public AnimationState(string _stateName) { stateName = _stateName; }

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

        public bool state = false;
        public float speed = 1f;
        public string stateName;
        List<AnimationState> trueConditionals = new List<AnimationState>();
        List<AnimationState> falseConditionals = new List<AnimationState>();
    }

    public class AnimationStateMachine
    {
        Animator animator;
        Dictionary<string, AnimationState> animationStates = new Dictionary<string, AnimationState>();
        AnimationState currState = null;

        public AnimationStateMachine (Animator _animator)
        {
            animator = _animator;
            animator.SetDefaultState("Idle");
            animator.Play();
        }

        //Call this in update function to update the animation state
        public void UpdateState()
        {
            foreach (var pair in animationStates)
            {
                //If this state can be transitioned to
                if (pair.Value.canTransition)
                {
                    AnimationState newState = pair.Value;
                    //If its the same state
                    SetState(newState);
                    return;
                }
            }

            if (currState != null)
            {
                currState = null;
                animator.SetState("Idle");
                animator.SetSpeed(1f);
            }
        }

        public AnimationState GetState(string name)
        {
            if (name == "")
                return null;
            if (!animationStates.ContainsKey(name))
                animationStates.Add(name, new AnimationState(name));
            return animationStates[name];
        }

        public void SetState(AnimationState state)
        {
            animator.SetState(state.stateName);
            currState = state;
            animator.SetSpeed(state.speed);
        }

        public AnimationState GetCurrentState()
        {
            return animationStates[currState.stateName];
        }

    }
}

