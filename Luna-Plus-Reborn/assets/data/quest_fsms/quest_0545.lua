--[[
  Quest 545: [Repeat][Quest] Hoods of the Black Widow
  Level Required: 150
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [544]
  Rewards: EXP=5000000
]]

local fsm = require('fsm_engine')

local quest_545 = fsm:new({
    id = 545,
    name = "[Repeat][Quest] Hoods of the Black Widow",
    level_required = 150,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {544},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 545: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 545: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 545: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 545: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 545: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 545: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 284,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 545: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 545: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 545: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 545: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 545: Rewards given")
                self:give_rewards({
                    exp = 5000000,
                    items = {
                        { item_id = 341, count = 1 },
                        { item_id = 341, count = 1 },
                        { item_id = 341, count = 1 },
                        { item_id = 341, count = 1 },
                        { item_id = 341, count = 1 },
                        { item_id = 30000003, count = 15 },
                        { item_id = 30000006, count = 15 },
                        { item_id = 30000009, count = 15 },
                        { item_id = 30000012, count = 15 },
                        { item_id = 30000015, count = 15 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_545