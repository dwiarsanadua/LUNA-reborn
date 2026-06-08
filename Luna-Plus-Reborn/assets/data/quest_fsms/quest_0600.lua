--[[
  Quest 600: Experiements in the secret Dungeon
  Level Required: 150
  NPC Start: 136, NPC Complete: 309
]]

local fsm = require('fsm_engine')

local quest_600 = fsm:new({
    id = 600,
    name = "Experiements in the secret Dungeon",
    level_required = 150,
    npc_start = 136,
    npc_complete = 309,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 600: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 136,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 600: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 600: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 603,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 604,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 605,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 606,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 607,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 136,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 304,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 305,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 306,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 307,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 308,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 309,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 600: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 600: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 309,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 600: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 600: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001192, count = 1 },
                        { item_id = 30000740, count = 1 },
                        { item_id = 30000741, count = 1 },
                        { item_id = 30000742, count = 1 },
                        { item_id = 30000743, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_600