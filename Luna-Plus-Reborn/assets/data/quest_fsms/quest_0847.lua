--[[
  Quest 847: Wreck of the gas
  Level Required: 86
  NPC Start: 505, NPC Complete: 505
  Prerequisites: [846]
  Rewards: EXP=3321421, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_847 = fsm:new({
    id = 847,
    name = "Wreck of the gas",
    level_required = 86,
    npc_start = 505,
    npc_complete = 505,
    prerequisites = {846},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 847: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 847: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 847: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 686,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 847: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 847: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 847: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 847: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 847: Rewards given")
                self:give_rewards({
                    exp = 3321421,
                    gold = 57826,
                    items = {
                        { item_id = 21000015, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_847