--[[
  Quest 706: Farm Repair
  Level Required: 60
  NPC Start: 45, NPC Complete: 45
  Rewards: EXP=729940
]]

local fsm = require('fsm_engine')

local quest_706 = fsm:new({
    id = 706,
    name = "Farm Repair",
    level_required = 60,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 706: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 706: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 706: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 706: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 706: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 706: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 706: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 706: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 706: Rewards given")
                self:give_rewards({
                    exp = 729940,
                    items = {
                        { item_id = 351, count = 1 },
                        { item_id = 351, count = 1 },
                        { item_id = 352, count = 1 },
                        { item_id = 352, count = 1 },
                        { item_id = 351, count = 30 },
                        { item_id = 352, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_706