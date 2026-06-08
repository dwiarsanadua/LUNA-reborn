--[[
  Quest 839: Hunger
  Level Required: 84
  NPC Start: 98, NPC Complete: 123
  Rewards: EXP=910000
]]

local fsm = require('fsm_engine')

local quest_839 = fsm:new({
    id = 839,
    name = "Hunger",
    level_required = 84,
    npc_start = 98,
    npc_complete = 123,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 839: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 839: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 839: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 839: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 839: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 839: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 839: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 839: Rewards given")
                self:give_rewards({
                    exp = 910000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_839