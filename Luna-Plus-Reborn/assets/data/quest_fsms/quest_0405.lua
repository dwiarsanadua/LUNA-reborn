--[[
  Quest 405: [Special] The Fall of Tarintus
  Level Required: 50
  NPC Start: 9, NPC Complete: 96
]]

local fsm = require('fsm_engine')

local quest_405 = fsm:new({
    id = 405,
    name = "[Special] The Fall of Tarintus",
    level_required = 50,
    npc_start = 9,
    npc_complete = 96,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 405: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 405: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 405: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 405: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 405: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 405: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 405: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 405: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 405: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 405: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 405: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000121, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_405