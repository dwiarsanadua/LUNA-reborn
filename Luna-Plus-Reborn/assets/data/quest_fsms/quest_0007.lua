--[[
  Quest 7: A Mothers Legacy
  Level Required: 11
  NPC Start: 3, NPC Complete: 3
  Rewards: EXP=1700, Gold=1215
]]

local fsm = require('fsm_engine')

local quest_7 = fsm:new({
    id = 7,
    name = "A Mothers Legacy",
    level_required = 11,
    npc_start = 3,
    npc_complete = 3,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 7: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 7: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 7: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 5,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 7: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 7: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 7: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 7: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 7: Rewards given")
                self:give_rewards({
                    exp = 1700,
                    gold = 1215,
                    items = {
                        { item_id = 2, count = 1 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_7