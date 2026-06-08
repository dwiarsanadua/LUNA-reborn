--[[
  Quest 143: [Repeatable] Bad Moon Rising
  Level Required: 23
  NPC Start: 26, NPC Complete: 26
  Prerequisites: [118]
  Rewards: EXP=10563, Gold=2060
]]

local fsm = require('fsm_engine')

local quest_143 = fsm:new({
    id = 143,
    name = "[Repeatable] Bad Moon Rising",
    level_required = 23,
    npc_start = 26,
    npc_complete = 26,
    prerequisites = {118},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 143: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 143: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 143: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 143: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 143: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 143: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 143: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 143: Rewards given")
                self:give_rewards({
                    exp = 10563,
                    gold = 2060,
                    items = {
                        { item_id = 21000282, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_143