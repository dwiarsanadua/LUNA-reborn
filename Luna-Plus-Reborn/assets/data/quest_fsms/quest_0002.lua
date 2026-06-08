--[[
  Quest 2: Battle Training
  Level Required: 1
  NPC Start: 2, NPC Complete: 2
  Prerequisites: [1]
  Rewards: EXP=7, Gold=885
]]

local fsm = require('fsm_engine')

local quest_2 = fsm:new({
    id = 2,
    name = "Battle Training",
    level_required = 1,
    npc_start = 2,
    npc_complete = 2,
    prerequisites = {1},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 2,
                    count = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 7,
                    count = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2: Rewards given")
                self:give_rewards({
                    exp = 7,
                    gold = 885,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2