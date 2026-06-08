--[[
  Quest 165: Stop the Orc Advance
  Level Required: 35
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [127, 160]
  Rewards: EXP=70695, Gold=6080
]]

local fsm = require('fsm_engine')

local quest_165 = fsm:new({
    id = 165,
    name = "Stop the Orc Advance",
    level_required = 35,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {127, 160},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 165: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 165: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 165: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 165: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 165: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 165: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 165: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 165: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 165: Rewards given")
                self:give_rewards({
                    exp = 70695,
                    gold = 6080,
                    items = {
                        { item_id = 21000004, count = 20 },
                        { item_id = 30000824, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_165