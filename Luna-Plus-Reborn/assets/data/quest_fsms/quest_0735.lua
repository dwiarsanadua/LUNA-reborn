--[[
  Quest 735: [Quest] Old Books
  Level Required: 57
  NPC Start: 92, NPC Complete: 117
  Prerequisites: [734]
  Rewards: EXP=531156
]]

local fsm = require('fsm_engine')

local quest_735 = fsm:new({
    id = 735,
    name = "[Quest] Old Books",
    level_required = 57,
    npc_start = 92,
    npc_complete = 117,
    prerequisites = {734},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 735: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 735: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 735: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 20,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 735: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 99,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 735: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 100,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 735: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 735: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 735: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 735: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 735: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 735: Rewards given")
                self:give_rewards({
                    exp = 531156,
                    items = {
                        { item_id = 375, count = 1 },
                        { item_id = 362, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_735