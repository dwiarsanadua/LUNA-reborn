--[[
  Quest 124: The Hook of Death
  Level Required: 1
  NPC Start: 27, NPC Complete: 27
  Prerequisites: [123]
  Rewards: EXP=19326, Gold=4145
]]

local fsm = require('fsm_engine')

local quest_124 = fsm:new({
    id = 124,
    name = "The Hook of Death",
    level_required = 1,
    npc_start = 27,
    npc_complete = 27,
    prerequisites = {123},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 124: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 124: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 124: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 45,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 124: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 124: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 124: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 124: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 124: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 124: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 124: Rewards given")
                self:give_rewards({
                    exp = 19326,
                    gold = 4145,
                    items = {
                        { item_id = 60, count = 1 },
                        { item_id = 21000006, count = 5 },
                        { item_id = 61, count = 1 },
                        { item_id = 62, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_124