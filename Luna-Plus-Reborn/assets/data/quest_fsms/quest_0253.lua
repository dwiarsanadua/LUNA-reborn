--[[
  Quest 253: The Temple of War
  Level Required: 150
  NPC Start: 55, NPC Complete: 55
]]

local fsm = require('fsm_engine')

local quest_253 = fsm:new({
    id = 253,
    name = "The Temple of War",
    level_required = 150,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 253: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 253: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 253: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 253: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 253: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 253: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 253: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 253: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 253: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 253: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000143, count = 1 },
                        { item_id = 21000144, count = 1 },
                        { item_id = 21000145, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_253