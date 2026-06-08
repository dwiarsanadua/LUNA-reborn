--[[
  Quest 902: What is that?
  Level Required: 80
  NPC Start: 97, NPC Complete: 316
  Rewards: EXP=1114618
]]

local fsm = require('fsm_engine')

local quest_902 = fsm:new({
    id = 902,
    name = "What is that?",
    level_required = 80,
    npc_start = 97,
    npc_complete = 316,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 902: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 902: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 902: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 902: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 902: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 902: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 902: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 902: Rewards given")
                self:give_rewards({
                    exp = 1114618,
                })
            end,
            transitions = {},
        },
    },
})

return quest_902