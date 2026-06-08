--[[
  Quest 899: Edged Dagger
  Level Required: 99
  NPC Start: 537, NPC Complete: 537
  Rewards: EXP=12686528, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_899 = fsm:new({
    id = 899,
    name = "Edged Dagger",
    level_required = 99,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 899: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 899: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 899: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 899: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 899: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 899: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 899: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 899: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 899: Rewards given")
                self:give_rewards({
                    exp = 12686528,
                    gold = 77721,
                    items = {
                        { item_id = 404, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_899