--[[
  Quest 358: [Special] War Trophies
  Level Required: 150
  NPC Start: 55, NPC Complete: 55
]]

local fsm = require('fsm_engine')

local quest_358 = fsm:new({
    id = 358,
    name = "[Special] War Trophies",
    level_required = 150,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 358: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 358: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 358: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 358: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 358: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 358: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 358: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 358: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 358: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 358: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 272, count = 1 },
                        { item_id = 274, count = 1 },
                        { item_id = 276, count = 1 },
                        { item_id = 30000450, count = 1 },
                        { item_id = 30000451, count = 1 },
                        { item_id = 30000452, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_358