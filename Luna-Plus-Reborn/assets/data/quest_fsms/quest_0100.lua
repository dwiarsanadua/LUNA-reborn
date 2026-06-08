--[[
  Quest 100: Special Delivery
  Level Required: 15
  NPC Start: 5, NPC Complete: 21
  Rewards: EXP=1761, Gold=915
]]

local fsm = require('fsm_engine')

local quest_100 = fsm:new({
    id = 100,
    name = "Special Delivery",
    level_required = 15,
    npc_start = 5,
    npc_complete = 21,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 100: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 100: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 100: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 100: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 100: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 100: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 100: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 100: Rewards given")
                self:give_rewards({
                    exp = 1761,
                    gold = 915,
                    items = {
                        { item_id = 43, count = 1 },
                        { item_id = 43, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_100