--[[
  Quest 84: Kobold's Request
  Level Required: 12
  NPC Start: 52, NPC Complete: 52
  Prerequisites: [86]
  Rewards: EXP=2143, Gold=1485
]]

local fsm = require('fsm_engine')

local quest_84 = fsm:new({
    id = 84,
    name = "Kobold's Request",
    level_required = 12,
    npc_start = 52,
    npc_complete = 52,
    prerequisites = {86},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 84: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 84: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 84: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 84: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 84: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 84: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 84: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 84: Rewards given")
                self:give_rewards({
                    exp = 2143,
                    gold = 1485,
                    items = {
                        { item_id = 33, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_84